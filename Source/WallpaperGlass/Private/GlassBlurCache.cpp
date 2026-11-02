#include "GlassBlurCache.h"

#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "MaterialDomain.h"
#include "MaterialShared.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "RenderingThread.h"
#include "RHI.h"
#include "Slate/WidgetRenderer.h"
#include "Styling/SlateBrush.h"
#include "UObject/StrongObjectPtr.h"
#include "Widgets/Images/SImage.h"

namespace WallpaperBlur
{
	constexpr int32 MaxSourceCount = 4;
	constexpr int32 MaxBlurCount = 4;
	constexpr int32 HalfKernelWidth = 16;
	constexpr float RadiusPrecision = 4.0f;
	constexpr float SigmaPrecision = 256.0f;

	template<typename T>
	T* AddExpression(UMaterial* Material)
	{
		T* Expression = NewObject<T>(Material);
		Material->GetEditorOnlyData()->ExpressionCollection.Expressions.Add(Expression);
		return Expression;
	}

	void AddInput(UMaterialExpressionCustom* Custom, FName Name, UMaterialExpression* Expression, int32 Output = 0)
	{
		FCustomInput& Input = Custom->Inputs.AddDefaulted_GetRef();
		Input.InputName = Name;
		Input.Input.Connect(Output, Expression);
	}

	UMaterialExpressionScalarParameter* ScalarParameter(UMaterial* Material, FName Name, float Default)
	{
		auto* Parameter = AddExpression<UMaterialExpressionScalarParameter>(Material);
		Parameter->ParameterName = Name;
		Parameter->DefaultValue = Default;
		return Parameter;
	}

	UMaterialExpressionTextureSampleParameter2D* TextureSample(UMaterial* Material, UTexture* Texture, UMaterialExpression* UV)
	{
		auto* Sample = AddExpression<UMaterialExpressionTextureSampleParameter2D>(Material);
		Sample->ParameterName = TEXT("Source");
		Sample->Texture = Texture;
		Sample->AutoSetSampleType();
		Sample->Coordinates.Connect(0, UV);
		Sample->AutomaticViewMipBias = false;
		return Sample;
	}

	UMaterial* NewBlitMaterial()
	{
		UMaterial* Material = NewObject<UMaterial>(GetTransientPackage(), NAME_None, RF_Transient);
		Material->MaterialDomain = MD_UI;
		Material->BlendMode = BLEND_Opaque;
		return Material;
	}

	void CompileMaterial(UMaterial* Material)
	{
		Material->PostEditChange();
		Material->CacheShaders(EMaterialShaderPrecompileMode::Background);
	}

	UMaterial* CreateCopyMaterial(UTexture* Source)
	{
		UMaterial* Material = NewBlitMaterial();
		auto* UV = AddExpression<UMaterialExpressionTextureCoordinate>(Material);
		Material->GetEditorOnlyData()->EmissiveColor.Connect(0, TextureSample(Material, Source, UV));
		CompileMaterial(Material);
		return Material;
	}

	UMaterial* CreateBlurMaterial(UTexture* LinearTexture)
	{
		UMaterial* Material = NewBlitMaterial();
		auto* UV = AddExpression<UMaterialExpressionTextureCoordinate>(Material);
		auto* Axis = AddExpression<UMaterialExpressionVectorParameter>(Material);
		Axis->ParameterName = TEXT("Axis");
		Axis->DefaultValue = FLinearColor::Black;
		auto* Mip = ScalarParameter(Material, TEXT("Mip"), 0.0f);
		auto* Sum = AddExpression<UMaterialExpressionCustom>(Material);
		Sum->OutputType = CMOT_Float3;
		Sum->Inputs.Reset();
		AddInput(Sum, TEXT("Sigma"), ScalarParameter(Material, TEXT("Sigma"), 0.5f));
		FString Code = TEXT("float3 sum = 0; float total = 0; float invSigma = 1 / max(Sigma, 0.01);\n");
		for (int32 Tap = -HalfKernelWidth; Tap <= HalfKernelWidth; ++Tap)
		{
			const float Offset = float(Tap) / HalfKernelWidth;
			auto* Coordinates = AddExpression<UMaterialExpressionCustom>(Material);
			Coordinates->Inputs.Reset();
			Coordinates->OutputType = CMOT_Float2;
			Coordinates->Code = FString::Printf(TEXT("return UV + Axis.xy * %.8f;"), Offset);
			AddInput(Coordinates, TEXT("UV"), UV);
			AddInput(Coordinates, TEXT("Axis"), Axis);
			auto* Sample = TextureSample(Material, LinearTexture, Coordinates);
			Sample->MipValueMode = TMVM_MipLevel;
			Sample->MipValue.Connect(0, Mip);
			const FString Name = FString::Printf(TEXT("Tap%d"), Tap + HalfKernelWidth);
			AddInput(Sum, *Name, Sample);
			Code += FString::Printf(TEXT("{ float w = exp(-0.5 * %.8f * invSigma * invSigma); sum += %s * w; total += w; }\n"), Offset * Offset, *Name);
		}
		Sum->Code = Code + TEXT("return sum / total;");
		Material->GetEditorOnlyData()->EmissiveColor.Connect(0, Sum);
		CompileMaterial(Material);
		return Material;
	}

	bool IsMaterialReady(UMaterial* Material)
	{
		const FMaterialResource* Resource = Material ? Material->GetMaterialResource(GMaxRHIFeatureLevel) : nullptr;
		return Resource && Resource->GetGameThreadShaderMap() && Resource->IsGameThreadShaderMapComplete();
	}

	UTextureRenderTarget2D* CreateTarget(FIntPoint Size, bool bGenerateMips)
	{
		auto* Target = NewObject<UTextureRenderTarget2D>(GetTransientPackage(), NAME_None, RF_Transient);
		Target->SRGB = false;
		Target->TargetGamma = 1.0f;
		Target->Filter = TF_Trilinear;
		Target->AddressX = Target->AddressY = TA_Clamp;
		Target->ClearColor = FLinearColor::Black;
		Target->bAutoGenerateMips = bGenerateMips;
		Target->MipsSamplerFilter = TF_Bilinear;
		Target->MipsAddressU = Target->MipsAddressV = TA_Clamp;
		Target->InitCustomFormat(Size.X, Size.Y, PF_FloatRGBA, true);
		Target->UpdateResourceImmediate(true);
		return Target;
	}

	void DrawMaterial(FWidgetRenderer& Renderer, UMaterialInterface* Material, UTextureRenderTarget2D* Target)
	{
		FSlateBrush Brush;
		Brush.DrawAs = ESlateBrushDrawType::Image;
		Brush.SetResourceObject(Material);
		Brush.TintColor = FLinearColor::White;
		// Immediate means enqueue now, not a CPU/GPU wait. The following mip command
		// must execute after this draw, so do not defer the widget renderer update.
		Renderer.DrawWidget(Target, SNew(SImage).Image(&Brush), FVector2D(Target->SizeX, Target->SizeY), 0.0f, false);
	}

	struct FSourceCopy
	{
		TStrongObjectPtr<UTexture> Source;
		TStrongObjectPtr<UTextureRenderTarget2D> Target;
		TStrongObjectPtr<UMaterial> Material;
		TStrongObjectPtr<UMaterialInstanceDynamic> Instance;
		FIntPoint Size = FIntPoint::ZeroValue;
		const FTextureResource* ResourceIdentity = nullptr;
		FGuid LightingGuid;
		int32 ResidentMips = 0;
		bool bStaticImage = false;
		uint64 Revision = 0;
		uint64 LastRenderedFrame = MAX_uint64;
		uint64 LastUse = 0;
	};

	struct FBlurredSource
	{
		TWeakObjectPtr<UTexture> Source;
		FIntPoint RadiusKey = FIntPoint::ZeroValue;
		int32 SigmaKey = 0;
		FIntPoint SourceSize = FIntPoint::ZeroValue;
		TStrongObjectPtr<UTextureRenderTarget2D> Horizontal;
		TStrongObjectPtr<UTextureRenderTarget2D> Vertical;
		TStrongObjectPtr<UMaterialInstanceDynamic> HorizontalInstance;
		TStrongObjectPtr<UMaterialInstanceDynamic> VerticalInstance;
		float SourceMip = 0.0f;
		uint64 SourceRevision = MAX_uint64;
		uint64 LastUsedFrame = MAX_uint64;
		uint64 LastUse = 0;
	};

	template<typename T>
	TUniquePtr<T> EvictOldest(TArray<TUniquePtr<T>>& Entries)
	{
		int32 Oldest = 0;
		for (int32 Index = 1; Index < Entries.Num(); ++Index)
		{
			if (Entries[Index]->LastUse < Entries[Oldest]->LastUse)
			{
				Oldest = Index;
			}
		}
		TUniquePtr<T> Removed = MoveTemp(Entries[Oldest]);
		Entries.RemoveAtSwap(Oldest);
		return Removed;
	}
}

struct FGlassBlurCache::FImpl
{
	using FSourceCopy = WallpaperBlur::FSourceCopy;
	using FBlurredSource = WallpaperBlur::FBlurredSource;

	FWidgetRenderer* Renderer = new FWidgetRenderer(false, true);
	TStrongObjectPtr<UTextureRenderTarget2D> MaterialTexture;
	TStrongObjectPtr<UMaterial> BlurMaterial;
	TArray<TUniquePtr<FSourceCopy>> Sources;
	TArray<TUniquePtr<FBlurredSource>> Blurred;
	uint64 UseCounter = 0;

	~FImpl()
	{
		BeginCleanup(Renderer);
	}

	bool PrepareBlurMaterial()
	{
		if (!BlurMaterial.IsValid())
		{
			MaterialTexture.Reset(WallpaperBlur::CreateTarget(FIntPoint(1, 1), false));
			BlurMaterial.Reset(WallpaperBlur::CreateBlurMaterial(MaterialTexture.Get()));
		}
		return WallpaperBlur::IsMaterialReady(BlurMaterial.Get());
	}

	FSourceCopy& FindSource(UTexture* Source, FIntPoint Size)
	{
		const UTexture2D* Texture2D = Cast<UTexture2D>(Source);
		const int32 ResidentMips = Texture2D ? Texture2D->GetNumResidentMips() : 0;
		for (const auto& Entry : Sources)
		{
			if (Entry->Source.Get() != Source)
			{
				continue;
			}
			if (Entry->Size != Size || Entry->ResourceIdentity != Source->GetResource()
				|| Entry->ResidentMips != ResidentMips || Entry->LightingGuid != Source->GetLightingGuid())
			{
				Entry->Size = Size;
				Entry->ResourceIdentity = Source->GetResource();
				Entry->ResidentMips = ResidentMips;
				Entry->LightingGuid = Source->GetLightingGuid();
				Entry->Target->ResizeTarget(Size.X, Size.Y);
				Entry->LastRenderedFrame = MAX_uint64;
			}
			Entry->LastUse = ++UseCounter;
			return *Entry;
		}
		if (Sources.Num() >= WallpaperBlur::MaxSourceCount)
		{
			const TUniquePtr<FSourceCopy> Removed = WallpaperBlur::EvictOldest(Sources);
			Blurred.RemoveAll([SourceToRemove = Removed->Source.Get()](const auto& Blur)
			{
				return Blur->Source.Get() == SourceToRemove;
			});
		}
		auto Entry = MakeUnique<FSourceCopy>();
		Entry->Source.Reset(Source);
		Entry->Size = Size;
		Entry->ResourceIdentity = Source->GetResource();
		Entry->ResidentMips = ResidentMips;
		Entry->LightingGuid = Source->GetLightingGuid();
		// Transient Texture2D objects may be updated in place with UpdateTextureRegions.
		// Only ordinary assets are treated as static; media, RTs and dynamic textures stay live.
		Entry->bStaticImage = Texture2D && !Source->HasAnyFlags(RF_Transient);
		Entry->Target.Reset(WallpaperBlur::CreateTarget(Size, true));
		Entry->Material.Reset(WallpaperBlur::CreateCopyMaterial(Source));
		Entry->Instance.Reset(UMaterialInstanceDynamic::Create(Entry->Material.Get(), GetTransientPackage()));
		Entry->Instance->SetTextureParameterValue(TEXT("Source"), Source);
		Entry->LastUse = ++UseCounter;
		return *Sources.Add_GetRef(MoveTemp(Entry));
	}

	TUniquePtr<FBlurredSource> AcquireBlurEntry()
	{
		if (Blurred.Num() < WallpaperBlur::MaxBlurCount)
		{
			return MakeUnique<FBlurredSource>();
		}
		int32 Reusable = INDEX_NONE;
		for (int32 Index = 0; Index < Blurred.Num(); ++Index)
		{
			// Static results can be used without rendering. Track use, not just render time,
			// to keep every texture already handed to this frame's nodes unchanged.
			if (Blurred[Index]->LastUsedFrame != GFrameCounter
				&& (Reusable == INDEX_NONE || Blurred[Index]->LastUse < Blurred[Reusable]->LastUse))
			{
				Reusable = Index;
			}
		}
		if (Reusable != INDEX_NONE)
		{
			TUniquePtr<FBlurredSource> Entry = MoveTemp(Blurred[Reusable]);
			Blurred.RemoveAtSwap(Reusable);
			return Entry;
		}
		// All slots were used this frame. Retire one without overwriting its target;
		// widgets retain that target until their next material update.
		WallpaperBlur::EvictOldest(Blurred);
		return MakeUnique<FBlurredSource>();
	}

	FBlurredSource& FindBlur(const FSourceCopy& Source, FIntPoint RadiusKey, int32 SigmaKey)
	{
		for (const auto& Entry : Blurred)
		{
			if (Entry->Source.Get() == Source.Source.Get() && Entry->SourceSize == Source.Size
				&& Entry->RadiusKey == RadiusKey && Entry->SigmaKey == SigmaKey)
			{
				Entry->LastUse = ++UseCounter;
				Entry->LastUsedFrame = GFrameCounter;
				return *Entry;
			}
		}
		auto Entry = AcquireBlurEntry();
		Entry->Source = Source.Source.Get();
		Entry->SourceSize = Source.Size;
		Entry->RadiusKey = RadiusKey;
		Entry->SigmaKey = SigmaKey;
		Entry->SourceRevision = MAX_uint64;
		// Each sample covers its spacing instead of skipping texels as the radius grows.
		const float Spacing = FMath::Max(RadiusKey.X, RadiusKey.Y) / (WallpaperBlur::RadiusPrecision * WallpaperBlur::HalfKernelWidth);
		const float RequiredMip = FMath::Max(0.0f, FMath::Log2(FMath::Max(Spacing, 1.0f)));
		const int32 Mip = FMath::Min(FMath::CeilToInt(RequiredMip), Source.Target->GetNumMips() - 1);
		// Match prefilter width to the smaller output so downsampling does not reintroduce aliasing.
		Entry->SourceMip = static_cast<float>(Mip);
		const FIntPoint Size(FMath::Max(Source.Size.X >> Mip, FMath::Min(Source.Size.X, 2)),
			FMath::Max(Source.Size.Y >> Mip, FMath::Min(Source.Size.Y, 2)));
		if (Entry->Horizontal.IsValid())
		{
			Entry->Horizontal->ResizeTarget(Size.X, Size.Y);
			Entry->Vertical->ResizeTarget(Size.X, Size.Y);
		}
		else
		{
			Entry->Horizontal.Reset(WallpaperBlur::CreateTarget(Size, false));
			Entry->Vertical.Reset(WallpaperBlur::CreateTarget(Size, false));
			Entry->HorizontalInstance.Reset(UMaterialInstanceDynamic::Create(BlurMaterial.Get(), GetTransientPackage()));
			Entry->VerticalInstance.Reset(UMaterialInstanceDynamic::Create(BlurMaterial.Get(), GetTransientPackage()));
		}
		Entry->LastUse = ++UseCounter;
		Entry->LastUsedFrame = GFrameCounter;
		return *Blurred.Add_GetRef(MoveTemp(Entry));
	}

	void Render(FSourceCopy& Source, FBlurredSource& Blur)
	{
		if (Source.LastRenderedFrame == MAX_uint64 || (!Source.bStaticImage && Source.LastRenderedFrame != GFrameCounter))
		{
			WallpaperBlur::DrawMaterial(*Renderer, Source.Instance.Get(), Source.Target.Get());
			// FWidgetRenderer does not generate RT mips after drawing.
			Source.Target->UpdateResourceImmediate(false);
			Source.LastRenderedFrame = GFrameCounter;
			++Source.Revision;
		}
		if (Blur.SourceRevision == Source.Revision)
		{
			return;
		}
		const float Sigma = Blur.SigmaKey / WallpaperBlur::SigmaPrecision;
		const FVector2D RadiusUV(Blur.RadiusKey.X / (WallpaperBlur::RadiusPrecision * Source.Size.X), Blur.RadiusKey.Y / (WallpaperBlur::RadiusPrecision * Source.Size.Y));
		Blur.HorizontalInstance->SetTextureParameterValue(TEXT("Source"), Source.Target.Get());
		Blur.HorizontalInstance->SetVectorParameterValue(TEXT("Axis"), FLinearColor(RadiusUV.X, 0, 0, 0));
		Blur.HorizontalInstance->SetScalarParameterValue(TEXT("Sigma"), Sigma);
		Blur.HorizontalInstance->SetScalarParameterValue(TEXT("Mip"), Blur.SourceMip);
		WallpaperBlur::DrawMaterial(*Renderer, Blur.HorizontalInstance.Get(), Blur.Horizontal.Get());
		Blur.VerticalInstance->SetTextureParameterValue(TEXT("Source"), Blur.Horizontal.Get());
		Blur.VerticalInstance->SetVectorParameterValue(TEXT("Axis"), FLinearColor(0, RadiusUV.Y, 0, 0));
		Blur.VerticalInstance->SetScalarParameterValue(TEXT("Sigma"), Sigma);
		Blur.VerticalInstance->SetScalarParameterValue(TEXT("Mip"), 0.0f);
		WallpaperBlur::DrawMaterial(*Renderer, Blur.VerticalInstance.Get(), Blur.Vertical.Get());
		Blur.SourceRevision = Source.Revision;
	}
};

FGlassBlurCache::FGlassBlurCache() = default;
FGlassBlurCache::~FGlassBlurCache() = default;

UTexture* FGlassBlurCache::GetBlurred(UTexture* Source, FVector2D RadiusUV, float Sigma)
{
	if (!IsValid(Source) || !FMath::IsFinite(RadiusUV.X) || !FMath::IsFinite(RadiusUV.Y) || !FMath::IsFinite(Sigma))
	{
		return nullptr;
	}
	const FIntPoint Size(FMath::RoundToInt(Source->GetSurfaceWidth()), FMath::RoundToInt(Source->GetSurfaceHeight()));
	if (Size.X <= 0 || Size.Y <= 0)
	{
		return nullptr;
	}
	if (!Impl)
	{
		Impl = MakeUnique<FImpl>();
	}
	const FIntPoint RadiusKey(FMath::RoundToInt(FMath::Clamp(RadiusUV.X, 0.0, 1.0) * Size.X * WallpaperBlur::RadiusPrecision),
		FMath::RoundToInt(FMath::Clamp(RadiusUV.Y, 0.0, 1.0) * Size.Y * WallpaperBlur::RadiusPrecision));
	const int32 SigmaKey = FMath::RoundToInt(FMath::Clamp(Sigma, 0.15f, 1.5f) * WallpaperBlur::SigmaPrecision);
	WallpaperBlur::FSourceCopy& Copy = Impl->FindSource(Source, Size);
	if (!Impl->PrepareBlurMaterial() || !WallpaperBlur::IsMaterialReady(Copy.Material.Get()))
	{
		return nullptr;
	}
	WallpaperBlur::FBlurredSource& Blur = Impl->FindBlur(Copy, RadiusKey, SigmaKey);
	Impl->Render(Copy, Blur);
	return Blur.Vertical.Get();
}
