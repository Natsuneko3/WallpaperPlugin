#include "Styling/WallpaperStyleApplier.h"

#include "Framework/Application/SlateApplication.h"
#include "Styling/AppStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateTypes.h"
#include "UObject/UnrealType.h"

DEFINE_LOG_CATEGORY_STATIC(LogWallpaperStyle, Log, All);

namespace
{
	bool IsStyleAvailable()
	{
		// UE 5.6 initializes Starship, not the legacy FCoreStyle singleton.
		// Match the registered style resolved by FAppStyle::Get().
		return FSlateApplication::IsInitialized() && FSlateStyleRegistry::FindSlateStyle(FAppStyle::GetAppStyleSetName()) != nullptr;
	}

	FWindowStyle& GetWindowStyle()
	{
		// Existing windows keep pointers to these brushes, so update the registered style in place.
		return const_cast<FWindowStyle&>(FAppStyle::Get().GetWidgetStyle<FWindowStyle>(TEXT("Window")));
	}

	FSlateBrush& GetBrush(const FName Name)
	{
		return *const_cast<FSlateBrush*>(FAppStyle::Get().GetBrush(Name));
	}

	FStyleColorList* GetActiveColors(USlateThemeManager& Manager)
	{
		// UE's theme editor edits this transient reflected property. SetDefaultColor changes
		// fallback defaults instead, and ApplyTheme would reload colors from disk.
		FStructProperty* Property = FindFProperty<FStructProperty>(Manager.GetClass(), TEXT("ActiveColors"));
		if (!Property || Property->Struct != FStyleColorList::StaticStruct())
		{
			UE_LOG(LogWallpaperStyle, Warning, TEXT("The active theme colors are unavailable; leaving the theme unchanged."));
			return nullptr;
		}
		return Property->ContainerPtrToValuePtr<FStyleColorList>(&Manager);
	}

	FLinearColor MakeOpacityTint(float Opacity)
	{
		return FLinearColor(1.0f, 1.0f, 1.0f, FMath::Clamp(Opacity, 0.0f, 1.0f));
	}

	void InvalidateStyle()
	{
		FSlateApplication::Get().InvalidateAllWidgets(false);
	}
}

FWallpaperStyleApplier::~FWallpaperStyleApplier()
{
	RestoreOriginalStyle();
}

void FWallpaperStyleApplier::CaptureOriginalStyle()
{
	if (bHasOriginalStyle || !IsStyleAvailable())
	{
		return;
	}

	const FWindowStyle& WindowStyle = GetWindowStyle();
	OriginalEditorBrush = WindowStyle.BackgroundBrush;
	OriginalPanelBrush = WindowStyle.ChildBackgroundBrush;
	OriginalMenuBrush = GetBrush(TEXT("Menu.Background"));
	OriginalGraphBrush = GetBrush(TEXT("Graph.Panel.SolidBackground"));
	ThemeManager = &USlateThemeManager::Get();
	OriginalThemeId = ThemeManager->GetCurrentThemeID();
	if (const FStyleColorList* Colors = GetActiveColors(*ThemeManager.Get()))
	{
		OriginalColors = *Colors;
		bHasOriginalColors = true;
	}
	bHasOriginalStyle = true;
}

void FWallpaperStyleApplier::RestoreOriginalStyle()
{
	if (!bHasOriginalStyle)
	{
		return;
	}

	if (IsStyleAvailable())
	{
		RestoreTheme();
		// Restore brushes after ApplyTheme, since its notifications can rebuild styles.
		FWindowStyle& WindowStyle = GetWindowStyle();
		WindowStyle.BackgroundBrush = OriginalEditorBrush;
		WindowStyle.ChildBackgroundBrush = OriginalPanelBrush;
		GetBrush(TEXT("Menu.Background")) = OriginalMenuBrush;
		GetBrush(TEXT("Graph.Panel.SolidBackground")) = OriginalGraphBrush;
		InvalidateStyle();
	}

	bHasOriginalStyle = false;
	bHasOriginalColors = false;
	ThemeManager.Reset();
	OriginalEditorBrush = FSlateBrush();
	OriginalPanelBrush = FSlateBrush();
	OriginalMenuBrush = FSlateBrush();
	OriginalGraphBrush = FSlateBrush();
	AppliedEditorBrush = FSlateBrush();
	AppliedPanelBrush = FSlateBrush();
	AppliedMenuBrush = FSlateBrush();
}

void FWallpaperStyleApplier::RestoreTheme()
{
	USlateThemeManager* Manager = ThemeManager.Get();
	if (!Manager)
	{
		return;
	}
	if (Manager->GetCurrentThemeID() != OriginalThemeId)
	{
		if (!Manager->DoesThemeExist(OriginalThemeId))
		{
			return;
		}
		Manager->ApplyTheme(OriginalThemeId);
	}
	if (FStyleColorList* Colors = bHasOriginalColors ? GetActiveColors(*Manager) : nullptr)
	{
		*Colors = OriginalColors;
	}
}

void FWallpaperStyleApplier::MakeGraphPanelTransparent()
{
	if (bHasOriginalStyle && IsStyleAvailable())
	{
		GetBrush(TEXT("Graph.Panel.SolidBackground")).TintColor = FLinearColor::Transparent;
	}
}

void FWallpaperStyleApplier::ApplyResource(UObject* Resource, EWallpaperTarget Target, float Opacity)
{
	if (!bHasOriginalStyle || !IsStyleAvailable() || !IsValid(Resource))
	{
		return;
	}

	FSlateBrush& AppliedBrush = Target == EWallpaperTarget::Editor ? AppliedEditorBrush : AppliedPanelBrush;
	AppliedBrush = FSlateBrush();
	AppliedBrush.SetResourceObject(Resource);
	AppliedBrush.SetImageSize(FVector2D(32, 32));
	AppliedBrush.TintColor = MakeOpacityTint(Opacity);
	FWindowStyle& WindowStyle = GetWindowStyle();
	FSlateBrush& WindowBrush = Target == EWallpaperTarget::Editor ? WindowStyle.BackgroundBrush : WindowStyle.ChildBackgroundBrush;
	WindowBrush = AppliedBrush;
	MakeGraphPanelTransparent();
	InvalidateStyle();
}

void FWallpaperStyleApplier::ApplyMenuBackground(const FSlateBrush& MenuBackground)
{
	if (!bHasOriginalStyle || !IsStyleAvailable())
	{
		return;
	}

	AppliedMenuBrush = MenuBackground;
	AppliedMenuBrush.SetImageSize(FVector2D(32, 32));
	GetBrush(TEXT("Menu.Background")) = AppliedMenuBrush;
	InvalidateStyle();
}

void FWallpaperStyleApplier::ApplyWindowOpacity(float EditorOpacity, float PanelOpacity)
{
	if (!bHasOriginalStyle || !IsStyleAvailable())
	{
		return;
	}

	FWindowStyle& WindowStyle = GetWindowStyle();
	WindowStyle.BackgroundBrush.TintColor = MakeOpacityTint(EditorOpacity);
	WindowStyle.ChildBackgroundBrush.TintColor = MakeOpacityTint(PanelOpacity);
	InvalidateStyle();
}

void FWallpaperStyleApplier::ApplyThemeColors(const FStyleColorList& StyleColors)
{
	if (!bHasOriginalStyle || !bHasOriginalColors || !IsStyleAvailable() || !ThemeManager.IsValid())
	{
		return;
	}
	FStyleColorList* ActiveColors = GetActiveColors(*ThemeManager.Get());
	if (!ActiveColors)
	{
		return;
	}

	bool bChanged = false;
	for (int32 ColorIndex = 0; ColorIndex < static_cast<int32>(EStyleColor::User1); ++ColorIndex)
	{
		if (ActiveColors->StyleColors[ColorIndex] != StyleColors.StyleColors[ColorIndex])
		{
			ActiveColors->StyleColors[ColorIndex] = StyleColors.StyleColors[ColorIndex];
			bChanged = true;
		}
	}
	if (bChanged)
	{
		InvalidateStyle();
	}
}

void FWallpaperStyleApplier::AddReferencedObjects(FReferenceCollector& Collector)
{
	OriginalEditorBrush.AddReferencedObjects(Collector);
	OriginalPanelBrush.AddReferencedObjects(Collector);
	OriginalMenuBrush.AddReferencedObjects(Collector);
	OriginalGraphBrush.AddReferencedObjects(Collector);
	AppliedEditorBrush.AddReferencedObjects(Collector);
	AppliedPanelBrush.AddReferencedObjects(Collector);
	AppliedMenuBrush.AddReferencedObjects(Collector);
}

FString FWallpaperStyleApplier::GetReferencerName() const
{
	return TEXT("FWallpaperStyleApplier");
}
