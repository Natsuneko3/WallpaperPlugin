#include "Styling/WallpaperStyleApplier.h"

#include "Engine/Texture.h"
#include "Materials/Material.h"
#include "Settings/EditorStyleSettings.h"
#include "Styling/AppStyle.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateTypes.h"

namespace
{
	FWindowStyle& GetWindowStyle()
	{
		auto& CoreStyles = FCoreStyle::Get();
		return (FWindowStyle&)CoreStyles.GetWidgetStyle<FWindowStyle>("Window");
	}
}

void FWallpaperStyleApplier::MakeGraphPanelTransparent()
{
	auto& EditorStyles = FAppStyle::Get();
	FSlateBrush& SolidBackground = *(FSlateBrush*)EditorStyles.GetBrush("Graph.Panel.SolidBackground");
	SolidBackground.TintColor = FLinearColor(0, 0, 0, 0);
}

void FWallpaperStyleApplier::ApplyMenuBackground(FSlateBrush MenuBackground)
{
	auto& CoreStyles = FCoreStyle::Get();
	FSlateBrush& MenuBackgroundBrush = *(FSlateBrush*)CoreStyles.GetBrush("Menu.Background");
	MenuBackground.ImageSize = FVector2D(32, 32);
	MenuBackgroundBrush = MenuBackground;
}

void FWallpaperStyleApplier::ApplyEditorTexture(UTexture* Texture, float Opacity)
{
	if (!Texture)
	{
		return;
	}

	FWindowStyle& WindowStyle = GetWindowStyle();
	WindowStyle.BackgroundBrush = MakeBrush(Texture);
	WindowStyle.BackgroundBrush.TintColor = MakeOpacityTint(Opacity);
}

void FWallpaperStyleApplier::ApplyPanelTexture(UTexture* Texture, float Opacity)
{
	if (!Texture)
	{
		return;
	}

	FWindowStyle& WindowStyle = GetWindowStyle();
	WindowStyle.ChildBackgroundBrush = MakeBrush(Texture);
	WindowStyle.ChildBackgroundBrush.TintColor = MakeOpacityTint(Opacity);
	MakeGraphPanelTransparent();
}

void FWallpaperStyleApplier::ApplyEditorMaterial(UMaterial* Material, float Opacity)
{
	if (!Material)
	{
		return;
	}

	FWindowStyle& WindowStyle = GetWindowStyle();
	WindowStyle.BackgroundBrush = MakeBrush(Material);
	WindowStyle.BackgroundBrush.TintColor = MakeOpacityTint(Opacity);
}

void FWallpaperStyleApplier::ApplyPanelMaterial(UMaterial* Material, float Opacity)
{
	if (!Material)
	{
		return;
	}

	FWindowStyle& WindowStyle = GetWindowStyle();
	WindowStyle.ChildBackgroundBrush = MakeBrush(Material);
	WindowStyle.ChildBackgroundBrush.TintColor = MakeOpacityTint(Opacity);
	MakeGraphPanelTransparent();
}

void FWallpaperStyleApplier::ApplyWindowOpacity(float EditorOpacity, float PanelOpacity)
{
	FWindowStyle& WindowStyle = GetWindowStyle();
	WindowStyle.BackgroundBrush.TintColor = MakeOpacityTint(EditorOpacity);
	WindowStyle.ChildBackgroundBrush.TintColor = MakeOpacityTint(PanelOpacity);
}

void FWallpaperStyleApplier::ApplyThemeColors(const FStyleColorList& StyleColors, FGuid& LastThemeId)
{
	USlateThemeManager* EditorTheme = &USlateThemeManager::Get();
	for (int ColorIndex = 0; ColorIndex < int(EStyleColor::User1); ColorIndex++)
	{
		const FLinearColor SettingColor = StyleColors.StyleColors[ColorIndex];
		const EStyleColor CurrentColor = EStyleColor(ColorIndex);

		EditorTheme->SetDefaultColor(CurrentColor, SettingColor);
		EditorTheme->ResetActiveColorToDefault(CurrentColor);
	}

	const FGuid NewGuid = EditorTheme->DuplicateActiveTheme();
	EditorTheme->ApplyTheme(NewGuid);
	EditorTheme->SetCurrentThemeDisplayName(FText::FromString(TEXT("WallpaperTheme")));
	EditorTheme->RemoveTheme(LastThemeId);
	LastThemeId = NewGuid;
	EditorTheme->ApplyDefaultTheme();
	EditorTheme->RemoveTheme(LastThemeId);
}

FSlateBrush FWallpaperStyleApplier::MakeBrush(UObject* Resource)
{
	FSlateBrush Brush;
	Brush.SetResourceObject(Resource);
	Brush.SetImageSize(FVector2D(32, 32));
	return Brush;
}

FLinearColor FWallpaperStyleApplier::MakeOpacityTint(float Opacity)
{
	return FLinearColor(FVector(Opacity));
}
