#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateBrush.h"
#include "Styling/StyleColors.h"

class UMaterial;
class UTexture;

class FWallpaperStyleApplier
{
public:
	static void MakeGraphPanelTransparent();
	static void ApplyMenuBackground(FSlateBrush MenuBackground);
	static void ApplyEditorTexture(UTexture* Texture, float Opacity);
	static void ApplyPanelTexture(UTexture* Texture, float Opacity);
	static void ApplyEditorMaterial(UMaterial* Material, float Opacity);
	static void ApplyPanelMaterial(UMaterial* Material, float Opacity);
	static void ApplyWindowOpacity(float EditorOpacity, float PanelOpacity);
	static void ApplyThemeColors(const FStyleColorList& StyleColors, FGuid& LastThemeId);

private:
	static FSlateBrush MakeBrush(UObject* Resource);
	static FLinearColor MakeOpacityTint(float Opacity);
};
