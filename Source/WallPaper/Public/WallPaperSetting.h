// Copyright 2023 Natsu Neko, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Styling/SlateBrush.h"
#include "Styling/StyleColors.h"
#include "UObject/Object.h"
#include "WallPaperSetting.generated.h"

UCLASS(Config = Wallpaper)
class WALLPAPER_API UWallPaperSetting : public UObject
{
	GENERATED_BODY()

public:
	UWallPaperSetting();

	UPROPERTY(Config, EditAnywhere, Category = "Engine Style", meta = (DisplayName = "Use Grid"))
	bool EditorUseGrid = false;

	UPROPERTY(Config, EditAnywhere, Category = "Engine Style", meta = (DisplayName = "Editor Opacity", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float EditorOpacity = 1.0f;

	UPROPERTY(Config, EditAnywhere, Category = "Engine Style", meta = (DisplayName = "Panel Opacity", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float PanelOpacity = 1.0f;

	UPROPERTY(Config, EditAnywhere, Category = "Engine Style", meta = (DisplayName = "Enable Timer"))
	bool bEnableTimer = true;

	UPROPERTY(Config, EditAnywhere, Category = "Engine Style", meta = (EditCondition = "bEnableTimer", DisplayName = "Wallpaper Loop Time (Min)", ClampMin = "1", UIMin = "1"))
	float Time = 30.0f;

	UPROPERTY(Config, EditAnywhere, Category = "Engine Style", meta = (DisplayName = "Use Wallpaper Engine"))
	bool UseWallpaperEngine = false;

	UPROPERTY(Config, EditAnywhere, Category = "Engine Style", meta = (DisplayName = "Steam Directory"))
	FDirectoryPath WallPaperDirectoryPath = FDirectoryPath(TEXT("D:/steam"));

	UPROPERTY(Config, EditAnywhere, Category = "Engine Style", meta = (DisplayName = "Menu Background"))
	FSlateBrush Menu_Background;

	UPROPERTY(Config, EditAnywhere, Category = "Color Style", meta = (DisplayName = "Theme"))
	FStyleColorList StyleColors;

	void ResetToDefaults();

private:
	void ResetStyleColor();
	void InitCoreStyle();
};
