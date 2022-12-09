// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "UObject/Object.h"
#include "Styling/StyleColors.h"
#include "WallPaperSetting.generated.h"

/**
 * 
 */
UCLASS(Config = Wallpaper)
class WALLPAPER_API UWallPaperSetting : public UObject
{
	GENERATED_BODY()

public:
	
	//UPROPERTY(Config, EditAnywhere, Category = "Engine Style", meta = (DisplayName = "Use Random Theme Color"))
	bool RandomMainThemeColor = false;
	
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

	UPROPERTY(Config, EditAnywhere, Category = "Engine Style", meta = (DisplayName = "Use Wallpaper Engine"))
	bool UseWallpaperEngine;
	
	UPROPERTY(Config, EditAnywhere, Category="Engine Style", meta = (DisplayName = "WallPaper Direction"))
	FDirectoryPath  WallPaperDirectoryPath;
	
	UPROPERTY(Config, EditAnywhere, Category = "Engine Style", meta = (DisplayName = "Menu Background"))
	FSlateBrush Menu_Background;

	UPROPERTY(Config, EditAnywhere, Category = "Color Style", meta = (DisplayName = "Theme"))
	FStyleColorList StyleColors;

	UWallPaperSetting();
	void ResetStyleColor();
	inline void PostProcessCustomStyle(UScriptStruct* StyleStruct, void* StructPtr);

public:
	
	void InitCoreStyle();
	
};
