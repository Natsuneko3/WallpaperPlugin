// Copyright 2023 Natsu Neko, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "UObject/Object.h"
#include "Styling/SlateTypes.h"
#include "WallPaperSetting.generated.h"

/**
 * 
 */
USTRUCT()
struct FEditorCustomizeDetailsView
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere,Category="DetailsViewStyle")
	FSlateBrush CategoryTop_Hovered;

	UPROPERTY(EditAnywhere, Category = "DetailsViewStyle")
	FSlateBrush CategoryTop;

	UPROPERTY(EditAnywhere, Category = "DetailsViewStyle")
	FSlateBrush CategoryMiddle_Highlighted;

	UPROPERTY(EditAnywhere, Category = "DetailsViewStyle")
	FSlateBrush CategoryMiddle_Hovered;

	UPROPERTY(EditAnywhere, Category = "DetailsViewStyle")
	FSlateBrush CategoryMiddle;

	UPROPERTY(EditAnywhere, Category = "DetailsViewStyle")
	FSlateBrush CollapsedCategory_Hovered;

	UPROPERTY(EditAnywhere, Category = "DetailsViewStyle")
	FSlateBrush CollapsedCategory;

	UPROPERTY(EditAnywhere, Category = "DetailsViewStyle")
	FSlateBrush CategoryBottom;

	UPROPERTY(EditAnywhere, Category = "DetailsViewStyle")
	FSlateBrush AdvancedDropdownBorder;
};
USTRUCT()
struct FUMGEditorPalette
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "UMGEditorPalette", meta = (DisplayName = "UMGEditor.PaletteHeader"))
	FTableRowStyle UMGEditor_PaletteHeader;

	UPROPERTY(EditAnywhere, Category = "UMGEditorPalette", meta = (DisplayName = "UMGEditor.PaletteItem"))
	FTableRowStyle UMGEditor_PaletteItem;
};
USTRUCT()
struct FGraphPanelStyle
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "GraphPanelStyle")
	FLinearColor GridLineColor;

	UPROPERTY(EditAnywhere, Category = "GraphPanelStyle")
	FLinearColor GridRuleColor;

	UPROPERTY(EditAnywhere, Category = "GraphPanelStyle")
	FLinearColor GridCenterColor;
};

UCLASS(Config = Wallpaper)
class WALLPAPER_API UWallPaperSetting : public UObject
{
	GENERATED_BODY()

public:
	
	

	UPROPERTY(Config, EditAnywhere, Category = "Engine Style", meta = (DisplayName = "Use Wallpaper Engine"))
	bool UseWallpaperEngine;
	
	UPROPERTY(Config, EditAnywhere, Category = "Engine Style", meta = (DisplayName = "UseGrid"))
	bool EditorUseGrid;
	
	UPROPERTY(Config,EditAnywhere,Category = "Engine Style",meta=(DisplayName = "Editor Opacity"))
	float EditorOpacity = 1;

	UPROPERTY(Config,EditAnywhere,Category = "Engine Style",meta=(DisplayName = "Panel Opacity"))
	float PanelOpacity = 1;

	UPROPERTY(config,EditAnywhere,Category = "Engine Style",meta=(DisplayName = "Enable Timer"))
	bool bEnableTimer;
	
	UPROPERTY(Config,EditAnywhere,Category = "Engine Style",meta=(EditCondition="bEnableTimer",DisplayName = "Wallpaper Loop Time(Min)"))
	float Time;
	
	UPROPERTY(Config, EditAnywhere, Category="Engine Style", meta = (DisplayName = "WallPaper Direction"))
	FDirectoryPath  WallPaperDirectoryPath;
	
	
	UPROPERTY(Config, EditAnywhere, Category = "Engine Style", meta = (DisplayName = "Menu Background"))
	FSlateBrush Menu_Background;



	///////////////////////////////////////////////////

	
	FDockTabStyle Docking_MajorTab;
	

	FDockTabStyle Docking_Tab;


	FSlateBrush ToolPanel_GroupBorder;
	

	FSlateBrush CategoryMiddle_Hovered;


	FSlateBrush ContentBrowser_TopBar_GroupBorder;

	FSlateBrush ToolBar_Background;

	FSlateBrush ToolPanel_DarkGroupBorder;
	FCheckBoxStyle PlacementBrowser_Tab;
	

	UWallPaperSetting();
	inline void PostProcessCustomStyle(UScriptStruct* StyleStruct, void* StructPtr);

public:
	
	void InitCoreStyle();
	void InitEditorStyle();
	
};
