#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateBrush.h"
#include "Styling/StyleColors.h"
#include "UObject/GCObject.h"
#include "WallpaperTarget.h"

/** Owns the resources and snapshots used while overriding the editor's shared styles. */
class FWallpaperStyleApplier : public FGCObject
{
public:
	FWallpaperStyleApplier() = default;
	virtual ~FWallpaperStyleApplier() override;
	FWallpaperStyleApplier(const FWallpaperStyleApplier&) = delete;
	FWallpaperStyleApplier& operator=(const FWallpaperStyleApplier&) = delete;

	void CaptureOriginalStyle();
	void RestoreOriginalStyle();
	void MakeGraphPanelTransparent();
	void ApplyResource(UObject* Resource, EWallpaperTarget Target, float Opacity);
	void ApplyMenuBackground(const FSlateBrush& MenuBackground);
	void ApplyWindowOpacity(float EditorOpacity, float PanelOpacity);
	void ApplyThemeColors(const FStyleColorList& StyleColors);

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;

private:
	void RestoreTheme();

	bool bHasOriginalStyle = false;
	bool bHasOriginalColors = false;
	TWeakObjectPtr<USlateThemeManager> ThemeManager;
	FGuid OriginalThemeId;
	FStyleColorList OriginalColors;
	FSlateBrush OriginalEditorBrush;
	FSlateBrush OriginalPanelBrush;
	FSlateBrush OriginalMenuBrush;
	FSlateBrush OriginalGraphBrush;
	FSlateBrush AppliedEditorBrush;
	FSlateBrush AppliedPanelBrush;
	FSlateBrush AppliedMenuBrush;
};
