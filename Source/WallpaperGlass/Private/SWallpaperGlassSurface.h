#pragma once

#include "CoreMinimal.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/StrongObjectPtr.h"
#include "Widgets/SCompoundWidget.h"

class SWindow;
class UWallpaperGlassSettings;

struct FGlassMaterialCache
{
    UMaterial* FindOrCreate(UTexture* Texture);
    UMaterial* FindOrCreateHighlight();
    TMap<uint8, TStrongObjectPtr<UMaterial>> Materials;
    TStrongObjectPtr<UMaterial> HighlightMaterial;
};

class SWallpaperGlassSurface : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SWallpaperGlassSurface) {}
        SLATE_ARGUMENT(TSharedPtr<SWindow>, Window)
        SLATE_ARGUMENT(TSharedPtr<FGlassMaterialCache>, Materials)
        SLATE_DEFAULT_SLOT(FArguments, Content)
    SLATE_END_ARGS()

    void Construct(const FArguments& Args);
	void RebindWindow(const TSharedRef<SWindow>& Window);
	TSharedPtr<SWindow> GetOwnerWindow() const;
    bool IsGlassReady() const { return bReady; }
    bool IsHighlightReady() const { return bHighlightReady; }
    void DetachFallback();
    virtual void Tick(const FGeometry& Geometry, double Time, float DeltaTime) override;
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& Geometry, const FSlateRect& CullRect,
        FSlateWindowElementList& Elements, int32 Layer, const FWidgetStyle& Style, bool bEnabled) const override;

private:
    bool UpdateMaterial(const FGeometry& Geometry, float DeltaTime);
    bool UpdateHighlight(const FGeometry& Geometry, float DeltaTime, const UWallpaperGlassSettings& Settings);
    TWeakPtr<SWindow> OwnerWindow;
    TSharedPtr<FGlassMaterialCache> Materials;
    TStrongObjectPtr<UMaterialInstanceDynamic> Instance;
    FSlateBrush Brush;
    TStrongObjectPtr<UMaterialInstanceDynamic> HighlightInstance;
    FSlateBrush HighlightBrush;
    FVector2D SmoothedMouse = FVector2D::ZeroVector;
    float HighlightPadding = 0;
    bool bHasMousePosition = false;
    bool bHighlightReady = false;
    bool bReady = false;
};
