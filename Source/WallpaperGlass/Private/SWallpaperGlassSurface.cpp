#include "SWallpaperGlassSurface.h"

#include "GlassMaterial.h"
#include "GlassUV.h"
#include "WallpaperGlassSettings.h"
#include "Framework/Application/SlateApplication.h"
#include "Materials/MaterialExpressionTextureBase.h"
#include "MaterialShared.h"
#include "RHI.h"
#include "Rendering/DrawElements.h"
#include "Styling/AppStyle.h"
#include "Widgets/SWindow.h"
#include "Widgets/SNullWidget.h"

namespace
{
	bool IsMaterialReady(UMaterial* Material)
	{
		const FMaterialResource* Resource = Material ? Material->GetMaterialResource(GMaxRHIFeatureLevel) : nullptr;
		return Resource && Resource->GetGameThreadShaderMap() && Resource->IsGameThreadShaderMapComplete();
	}
}

UMaterial* FGlassMaterialCache::FindOrCreate(UTexture* Texture)
{
	const uint8 Type = UMaterialExpressionTextureBase::GetSamplerTypeForTexture(Texture);
	if (auto* Found = Materials.Find(Type))
	{
		return Found->Get();
	}
	UMaterial* Material = CreateGlassMaterial(Texture);
	if (Material)
	{
		Materials.Add(Type, TStrongObjectPtr<UMaterial>(Material));
	}
	return Material;
}

UMaterial* FGlassMaterialCache::FindOrCreateHighlight()
{
	if (!HighlightMaterial.IsValid())
	{
		HighlightMaterial.Reset(CreateGlassHighlightMaterial());
	}
	return HighlightMaterial.Get();
}

void SWallpaperGlassSurface::Construct(const FArguments& Args)
{
	OwnerWindow = Args._Window;
	Materials = Args._Materials;
	ChildSlot[Args._Content.Widget];
	SetVisibility(EVisibility::HitTestInvisible);
	SetCanTick(true);
	// Video pixels change without changes to the Slate widget tree.
	ForceVolatile(true);
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.TintColor = FLinearColor::White;
	HighlightBrush.DrawAs = ESlateBrushDrawType::Image;
	HighlightBrush.TintColor = FLinearColor::White;
}

void SWallpaperGlassSurface::RebindWindow(const TSharedRef<SWindow>& Window)
{
	if (OwnerWindow.Pin() == Window)
	{
		return;
	}
	OwnerWindow = Window;
	// A docked tab keeps its widgets, but video UVs and cursor coordinates now
	// belong to a different window. Recompute them before painting glass again.
	bReady = false;
	bHighlightReady = false;
	bHasMousePosition = false;
	Invalidate(EInvalidateWidgetReason::Paint);
}

TSharedPtr<SWindow> SWallpaperGlassSurface::GetOwnerWindow() const
{
	return OwnerWindow.Pin();
}

bool SWallpaperGlassSurface::UpdateMaterial(const FGeometry& Geometry, float DeltaTime)
{
	const UWallpaperGlassSettings* Settings = GetDefault<UWallpaperGlassSettings>();
	const TSharedPtr<SWindow> Window = OwnerWindow.Pin();
	if (!Settings->bEnabled || !Window || Window->HasOSWindowBorder())
	{
		bHasMousePosition = false;
		return false;
	}

	// SWindow stretches its background over the entire window overlay (not the client rect).
	const FWindowStyle& WindowStyle = FAppStyle::Get().GetWidgetStyle<FWindowStyle>(TEXT("Window"));
	const FSlateBrush& Background = Window->GetParentWindow().IsValid()
		? WindowStyle.ChildBackgroundBrush : WindowStyle.BackgroundBrush;
	UTexture* Texture = Cast<UTexture>(Background.GetResourceObject());
	if (UMaterialInterface* Wallpaper = Cast<UMaterialInterface>(Background.GetResourceObject()))
	{
		Wallpaper->GetTextureParameterValue(FMaterialParameterInfo(TEXT("Texture")), Texture);
	}
	if (!Texture)
	{
		return false;
	}
	FGlassUV UV;
	if (!FGlassUV::Calculate(Geometry, Window->GetWindowGeometryInScreen(), UV))
	{
		return false;
	}
	UMaterial* Parent = Materials->FindOrCreate(Texture);
	if (!IsMaterialReady(Parent))
	{
		return false;
	}
	if (!Instance.IsValid() || Instance->Parent != Parent)
	{
		Instance.Reset(UMaterialInstanceDynamic::Create(Parent, GetTransientPackage()));
		Brush.SetResourceObject(Instance.Get());
	}
	Instance->SetTextureParameterValue(TEXT("Wallpaper"), Texture);
	const FVector2D Size = Geometry.GetLocalSize();
	if (Size.X <= UE_SMALL_NUMBER || Size.Y <= UE_SMALL_NUMBER)
	{
		return false;
	}
	const bool bDiagnostic = Settings->bShowUndistortedVideo;
	const float BlurRadius = bDiagnostic ? 0 : FMath::Clamp(Settings->BlurRadius, 0.0f, 32.0f);
	const FVector2D BlurX = UV.AxisX / Size.X * BlurRadius;
	const FVector2D BlurY = UV.AxisY / Size.Y * BlurRadius;
	const FVector2D ScatterX = UV.AxisX / Size.X * (bDiagnostic ? 0 : Settings->ScatteringRadius);
	const FVector2D ScatterY = UV.AxisY / Size.Y * (bDiagnostic ? 0 : Settings->ScatteringRadius);
	Instance->SetVectorParameterValue(TEXT("Mapping"), FLinearColor(UV.Origin.X, UV.Origin.Y, 0, 0));
	Instance->SetVectorParameterValue(TEXT("Axes"), FLinearColor(UV.AxisX.X, UV.AxisX.Y, UV.AxisY.X, UV.AxisY.Y));
	Instance->SetVectorParameterValue(TEXT("Size"), FLinearColor(Size.X, Size.Y, 0, 0));
	Instance->SetVectorParameterValue(TEXT("Shape"), FLinearColor(Settings->CornerRadius, Settings->EdgeWidth, bDiagnostic ? 0 : Settings->Refraction, 0));
	Instance->SetVectorParameterValue(TEXT("BlurStep"), FLinearColor(BlurX.X, BlurX.Y, BlurY.X, BlurY.Y));
	Instance->SetVectorParameterValue(TEXT("ScatterStep"), FLinearColor(ScatterX.X, ScatterX.Y, ScatterY.X, ScatterY.Y));
	Instance->SetVectorParameterValue(TEXT("Gaussian"), FLinearColor(
		BlurRadius > 0 ? FMath::Clamp(Settings->GaussianStrength, 0.0f, 1.0f) : 0,
		FMath::Clamp(Settings->GaussianSigma, 0.15f, 1.5f),
		FMath::Clamp(Settings->GaussianEdgeRetention, 0.0f, 1.0f), 0));
	Instance->SetVectorParameterValue(TEXT("Spectrum"), FLinearColor(
		bDiagnostic ? 0 : FMath::Clamp(Settings->DispersionRadius, 0.0f, 48.0f),
		bDiagnostic ? 0 : FMath::Clamp(Settings->DispersionStrength, 0.0f, 1.0f),
		FMath::Clamp(Settings->DispersionEdgePower, 0.25f, 4.0f), 0));
	Instance->SetVectorParameterValue(TEXT("DispersionShape"), FLinearColor(
		FMath::Clamp(Settings->DispersionEdgeWidth, 1.0f, 96.0f), 0, 0, 0));
	const FLinearColor WallpaperTint = Background.TintColor.GetSpecifiedColor() * WindowStyle.BackgroundColor.GetSpecifiedColor();
	Instance->SetVectorParameterValue(TEXT("Absorption"), WallpaperTint * (bDiagnostic ? FLinearColor::White : Settings->AbsorptionColor));
	const float Scattering = bDiagnostic || Settings->ScatteringRadius <= 0 ? 0 : Settings->ScatteringStrength;
	Instance->SetVectorParameterValue(TEXT("Optics"), FLinearColor(Scattering,
		bDiagnostic ? 1 : Settings->Transmission, 0, 0));
	// Wallpaper alpha already contributed to the backdrop. Glass coverage is independent.
	Instance->SetVectorParameterValue(TEXT("Finish"), FLinearColor(bDiagnostic ? 0 : Settings->HighlightStrength,
		bDiagnostic ? 1 : Settings->GlassOpacity, bDiagnostic ? 0 : 1, 0));
	bHighlightReady = UpdateHighlight(Geometry, DeltaTime, *Settings);
	return true;
}

bool SWallpaperGlassSurface::UpdateHighlight(const FGeometry& Geometry, float DeltaTime,
	const UWallpaperGlassSettings& Settings)
{
	if (!Settings.bCursorHighlight || Settings.bShowUndistortedVideo || Settings.CursorHighlightStrength <= 0)
	{
		bHasMousePosition = false;
		return false;
	}
	UMaterial* Parent = Materials->FindOrCreateHighlight();
	if (!IsMaterialReady(Parent)) return false;
	if (!HighlightInstance.IsValid() || HighlightInstance->Parent != Parent)
	{
		HighlightInstance.Reset(UMaterialInstanceDynamic::Create(Parent, GetTransientPackage()));
		HighlightBrush.SetResourceObject(HighlightInstance.Get());
	}

	const FVector2D Mouse = Geometry.AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());
	if (!bHasMousePosition)
	{
		SmoothedMouse = Mouse;
		bHasMousePosition = true;
	}
	const float FollowSpeed = FMath::Clamp(Settings.CursorFollowSpeed, 1.0f, 40.0f);
	SmoothedMouse += (Mouse - SmoothedMouse) * (1 - FMath::Exp(-FollowSpeed * FMath::Max(DeltaTime, 0.0f)));
	const float Width = FMath::Clamp(Settings.CursorHighlightWidth, 0.5f, 6.0f);
	const float GlowRadius = FMath::Clamp(Settings.CursorGlowRadius, 0.0f, 32.0f);
	HighlightPadding = FMath::Max(GlowRadius, Width + 2.0f);
	SetCullingBoundsExtension(FMargin(HighlightPadding));
	const FVector2D Size = Geometry.GetLocalSize();
	HighlightInstance->SetVectorParameterValue(TEXT("Size"), FLinearColor(Size.X, Size.Y, HighlightPadding, Settings.CornerRadius));
	HighlightInstance->SetVectorParameterValue(TEXT("Mouse"), FLinearColor(SmoothedMouse.X, SmoothedMouse.Y, 0, 0));
	HighlightInstance->SetVectorParameterValue(TEXT("Rim"), FLinearColor(Width,
		FMath::Clamp(Settings.CursorHighlightStrength, 0.0f, 4.0f),
		FMath::Clamp(Settings.CursorHighlightRadius, 16.0f, 600.0f), Settings.CursorHighlightAmbient));
	HighlightInstance->SetVectorParameterValue(TEXT("Glow"), FLinearColor(GlowRadius,
		GlowRadius > 0 ? FMath::Clamp(Settings.CursorGlowStrength, 0.0f, 2.0f) : 0, 0, 0));
	HighlightInstance->SetVectorParameterValue(TEXT("HighlightColor"), Settings.CursorHighlightColor);
	return true;
}

void SWallpaperGlassSurface::Tick(const FGeometry& Geometry, double Time, float DeltaTime)
{
	SCompoundWidget::Tick(Geometry, Time, DeltaTime);
	bHighlightReady = false;
	bReady = UpdateMaterial(Geometry, DeltaTime);
}

void SWallpaperGlassSurface::DetachFallback()
{
	ChildSlot[SNullWidget::NullWidget];
}

int32 SWallpaperGlassSurface::OnPaint(const FPaintArgs& Args, const FGeometry& Geometry, const FSlateRect& CullRect,
	FSlateWindowElementList& Elements, int32 Layer, const FWidgetStyle& Style, bool bEnabled) const
{
	if (!bReady || !Instance.IsValid())
	{
		return SCompoundWidget::OnPaint(Args, Geometry, CullRect, Elements, Layer, Style, bEnabled);
	}
	FSlateDrawElement::MakeBox(Elements, Layer, Geometry.ToPaintGeometry(), &Brush,
		ESlateDrawEffect::None, Style.GetColorAndOpacityTint());
	if (bHighlightReady && HighlightInstance.IsValid())
	{
		// Paint outside the body without affecting layout, hit testing or graph clipping.
		FSlateDrawElement::MakeBox(Elements, ++Layer, Geometry.ToInflatedPaintGeometry(FVector2D(HighlightPadding)),
			&HighlightBrush, ESlateDrawEffect::None, Style.GetColorAndOpacityTint());
	}
	return Layer;
}
