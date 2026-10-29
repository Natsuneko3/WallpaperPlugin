#include "GlassMaterial.h"

#include "Engine/Texture2D.h"
#include "Interfaces/IPluginManager.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionComponentMask.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

namespace
{
	template<typename T>
	T* AddExpression(UMaterial* Material)
	{
		T* Expression = NewObject<T>(Material);
		Material->GetEditorOnlyData()->ExpressionCollection.Expressions.Add(Expression);
		return Expression;
	}

	UMaterialExpressionVectorParameter* Parameter(UMaterial* Material, const TCHAR* Name, FLinearColor Default)
	{
		auto* Result = AddExpression<UMaterialExpressionVectorParameter>(Material);
		Result->ParameterName = Name;
		Result->DefaultValue = Default;
		Result->GetOutputs().Add(FExpressionOutput(TEXT("RGBA")));
		return Result;
	}

	void Input(UMaterialExpressionCustom* Custom, const TCHAR* Name, UMaterialExpression* Expression, int32 Index = 0)
	{
		FCustomInput& Entry = Custom->Inputs.AddDefaulted_GetRef();
		Entry.InputName = Name;
		Entry.Input.Connect(Index, Expression);
	}

	void Output(UMaterialExpressionCustom* Custom, const TCHAR* Name, ECustomMaterialOutputType Type = CMOT_Float1)
	{
		FCustomOutput& Entry = Custom->AdditionalOutputs.AddDefaulted_GetRef();
		Entry.OutputName = Name;
		Entry.OutputType = Type;
	}

	UMaterialExpressionCustom* CreateGlassGeometry(UMaterial* Material, const FString& Code)
	{
		auto* Geometry = AddExpression<UMaterialExpressionCustom>(Material);
		Geometry->Inputs.Reset();
		Geometry->Code = Code;
		Geometry->OutputType = CMOT_Float2;
		Input(Geometry, TEXT("UV"), AddExpression<UMaterialExpressionTextureCoordinate>(Material));
		// Vector parameters' first output is RGB; output 5 is RGBA.
		Input(Geometry, TEXT("Mapping"), Parameter(Material, TEXT("Mapping"), FLinearColor::Black), 5);
		Input(Geometry, TEXT("Axes"), Parameter(Material, TEXT("Axes"), FLinearColor(1,0,0,1)), 5);
		Input(Geometry, TEXT("Size"), Parameter(Material, TEXT("Size"), FLinearColor(100,100,0,0)), 5);
		Input(Geometry, TEXT("Shape"), Parameter(Material, TEXT("Shape"), FLinearColor(12,10,5,0)), 5);
		Input(Geometry, TEXT("DispersionShape"), Parameter(Material, TEXT("DispersionShape"), FLinearColor(24,0,0,0)), 5);
		Output(Geometry, TEXT("Mask"));
		Output(Geometry, TEXT("Highlight"));
		Output(Geometry, TEXT("Edge"));
		Output(Geometry, TEXT("NormalUV"), CMOT_Float2);
		Output(Geometry, TEXT("DispersionEdge"));
		Geometry->RebuildOutputs();
		return Geometry;
	}

	struct FGlassSampleInputs
	{
		UMaterial* Material;
		UTexture* Texture;
		UTexture* FilteredTexture;
		UMaterialExpressionCustom* Geometry;
		UMaterialExpressionCustom* Composite;
		UMaterialExpressionVectorParameter* ScatterStep;
		UMaterialExpressionVectorParameter* Spectrum;
	};

	void AddWallpaperSample(const FGlassSampleInputs& Inputs, const TCHAR* Name, const TCHAR* Offset)
	{
		auto* UV = AddExpression<UMaterialExpressionCustom>(Inputs.Material);
		UV->Inputs.Reset();
		UV->OutputType = CMOT_Float2;
		UV->Code = FString::Printf(TEXT("return clamp(UV + %s, 0.0001, 0.9999);"), Offset);
		Input(UV, TEXT("UV"), Inputs.Geometry);
		Input(UV, TEXT("Scatter"), Inputs.ScatterStep, 5);
		Input(UV, TEXT("Normal"), Inputs.Geometry, 4);
		Input(UV, TEXT("DispersionEdge"), Inputs.Geometry, 5);
		Input(UV, TEXT("Spectrum"), Inputs.Spectrum, 5);
		auto* Sample = AddExpression<UMaterialExpressionTextureSampleParameter2D>(Inputs.Material);
		Sample->ParameterName = TEXT("Wallpaper");
		Sample->Texture = Inputs.Texture;
		Sample->AutoSetSampleType();
		Sample->Coordinates.Connect(0, UV);
		Input(Inputs.Composite, Name, Sample);
		// Every optical branch uses the same preblurred source. Keep the original
		// sample only for the explicit Gaussian strength/edge blend and diagnostics.
		auto* Filtered = AddExpression<UMaterialExpressionTextureSampleParameter2D>(Inputs.Material);
		Filtered->ParameterName = TEXT("FilteredWallpaper");
		Filtered->Texture = Inputs.FilteredTexture;
		Filtered->AutoSetSampleType();
		Filtered->Coordinates.Connect(0, UV);
		Input(Inputs.Composite, *(FString(TEXT("Filtered")) + Name), Filtered);
	}

	void AddScatteringSamples(const FGlassSampleInputs& Inputs)
	{
		const TCHAR* Offsets[] = {
			TEXT("Scatter.xy"), TEXT("-Scatter.xy"), TEXT("Scatter.zw"), TEXT("-Scatter.zw"),
			TEXT("(Scatter.xy + Scatter.zw) * 0.70710678"), TEXT("(Scatter.xy - Scatter.zw) * 0.70710678"),
			TEXT("(-Scatter.xy + Scatter.zw) * 0.70710678"), TEXT("(-Scatter.xy - Scatter.zw) * 0.70710678") };
		for (int32 Index = 0; Index < UE_ARRAY_COUNT(Offsets); ++Index)
		{
			const FString Name = FString::Printf(TEXT("Scatter%d"), Index);
			AddWallpaperSample(Inputs, *Name, Offsets[Index]);
		}
	}

	void AddSpectrumSamples(const FGlassSampleInputs& Inputs)
	{
		constexpr int32 BandCount = 9;
		for (int32 Index = 0; Index < BandCount; ++Index)
		{
			const float BandOffset = 2.0f * Index / (BandCount - 1) - 1.0f;
			const FString Name = FString::Printf(TEXT("Spectrum%d"), Index);
			const FString Offset = FString::Printf(TEXT("Normal * max(Spectrum.x, 0) * %.2f * "
				"pow(saturate(DispersionEdge), max(Spectrum.z, 0.01))"), BandOffset);
			AddWallpaperSample(Inputs, *Name, *Offset);
		}
	}

	UMaterialExpressionCustom* CreateGlassComposite(UMaterial* Material, UTexture* Texture,
		UMaterialExpressionCustom* Geometry, const FString& Code)
	{
		auto* Composite = AddExpression<UMaterialExpressionCustom>(Material);
		Composite->Inputs.Reset();
		Composite->OutputType = CMOT_Float4;
		Composite->Code = Code;
		// The shared blur output is linear, including when the source is an sRGB
		// image or external media texture. Give its parameter a matching default.
		UTexture2D* FilteredTexture = UTexture2D::CreateTransient(1, 1);
		FilteredTexture->SRGB = false;
		FilteredTexture->UpdateResource();
		FGlassSampleInputs Inputs = { Material, Texture, FilteredTexture, Geometry, Composite,
			Parameter(Material, TEXT("ScatterStep"), FLinearColor::Black),
			Parameter(Material, TEXT("Spectrum"), FLinearColor(10,0.85f,1.2f,0)) };
		AddWallpaperSample(Inputs, TEXT("Center"), TEXT("float2(0, 0)"));
		AddScatteringSamples(Inputs);
		AddSpectrumSamples(Inputs);
		Input(Composite, TEXT("Mask"), Geometry, 1);
		Input(Composite, TEXT("Highlight"), Geometry, 2);
		Input(Composite, TEXT("Edge"), Geometry, 3);
		Input(Composite, TEXT("DispersionEdge"), Geometry, 5);
		Input(Composite, TEXT("Absorption"), Parameter(Material, TEXT("Absorption"), FLinearColor::White), 5);
		Input(Composite, TEXT("Optics"), Parameter(Material, TEXT("Optics"), FLinearColor(0.55f,0.82f,0,0)), 5);
		Input(Composite, TEXT("Gaussian"), Parameter(Material, TEXT("Gaussian"), FLinearColor(0.85f,0.5f,0.15f,0)), 5);
		Input(Composite, TEXT("Spectrum"), Inputs.Spectrum, 5);
		Input(Composite, TEXT("Finish"), Parameter(Material, TEXT("Finish"), FLinearColor(0.35f,0.92f,1,0)), 5);
		return Composite;
	}
}

UMaterial* CreateGlassMaterial(UTexture* Texture)
{
	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("WallPaper"));
	FString GeometryCode;
	FString CompositeCode;
	if (!Texture || !Plugin || !FFileHelper::LoadFileToString(GeometryCode,
		*FPaths::Combine(Plugin->GetBaseDir(), TEXT("Resources/GlassGeometry.hlsl")))
		|| !FFileHelper::LoadFileToString(CompositeCode,
		*FPaths::Combine(Plugin->GetBaseDir(), TEXT("Resources/GlassComposite.hlsl"))))
	{
		return nullptr;
	}

	UMaterial* Material = NewObject<UMaterial>(GetTransientPackage(), NAME_None, RF_Transient);
	Material->MaterialDomain = MD_UI;
	Material->BlendMode = BLEND_Translucent;
	auto* Geometry = CreateGlassGeometry(Material, GeometryCode);
	auto* Composite = CreateGlassComposite(Material, Texture, Geometry, CompositeCode);
	Material->GetEditorOnlyData()->EmissiveColor.Connect(0, Composite);
	auto* Alpha = AddExpression<UMaterialExpressionComponentMask>(Material);
	Alpha->Input.Connect(0, Composite);
	Alpha->R = Alpha->G = Alpha->B = false;
	Alpha->A = true;
	Material->GetEditorOnlyData()->Opacity.Connect(0, Alpha);
	Material->PostEditChange();
	Material->CacheShaders(EMaterialShaderPrecompileMode::Background);
	return Material;
}

UMaterial* CreateGlassHighlightMaterial()
{
	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("WallPaper"));
	FString Code;
	if (!Plugin || !FFileHelper::LoadFileToString(Code,
		*FPaths::Combine(Plugin->GetBaseDir(), TEXT("Resources/GlassHighlight.hlsl"))))
	{
		return nullptr;
	}

	UMaterial* Material = NewObject<UMaterial>(GetTransientPackage(), NAME_None, RF_Transient);
	Material->MaterialDomain = MD_UI;
	Material->BlendMode = BLEND_Additive;
	auto* Highlight = AddExpression<UMaterialExpressionCustom>(Material);
	Highlight->Inputs.Reset();
	Highlight->Code = Code;
	Highlight->OutputType = CMOT_Float4;
	Input(Highlight, TEXT("UV"), AddExpression<UMaterialExpressionTextureCoordinate>(Material));
	Input(Highlight, TEXT("Size"), Parameter(Material, TEXT("Size"), FLinearColor(100,100,10,12)), 5);
	Input(Highlight, TEXT("Mouse"), Parameter(Material, TEXT("Mouse"), FLinearColor::Black), 5);
	Input(Highlight, TEXT("Rim"), Parameter(Material, TEXT("Rim"), FLinearColor(1.4f,1.2f,180,0.12f)), 5);
	Input(Highlight, TEXT("Glow"), Parameter(Material, TEXT("Glow"), FLinearColor(10,0.3f,0,0)), 5);
	Input(Highlight, TEXT("HighlightColor"), Parameter(Material, TEXT("HighlightColor"), FLinearColor(0.2f,0.85f,0.75f,1)), 5);
	Material->GetEditorOnlyData()->EmissiveColor.Connect(0, Highlight);
	auto* Alpha = AddExpression<UMaterialExpressionComponentMask>(Material);
	Alpha->Input.Connect(0, Highlight);
	Alpha->R = Alpha->G = Alpha->B = false;
	Alpha->A = true;
	Material->GetEditorOnlyData()->Opacity.Connect(0, Alpha);
	Material->PostEditChange();
	Material->CacheShaders(EMaterialShaderPrecompileMode::Background);
	return Material;
}
