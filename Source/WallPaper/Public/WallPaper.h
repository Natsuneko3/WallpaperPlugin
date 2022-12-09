// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"
#include "ToolMenus.h"
#include "WallPaperEditor.h"


class UWindowsTargetSettings;
class UWallPaperSetting;

class FWallPaperModule : public IModuleInterface
{
	TSharedPtr<ISettingsSection> SettingS;

	//TODO 主题色，自动连接wallpaper。禁止删除tex
public:
	/** IModuleInterface implementation */
	//FWallPaperModule();
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	bool OnSettingModified();

	UWallPaperSetting* StyleSettings = GetMutableDefault<UWallPaperSetting>();
	void PluginButtonClicked();
	void ApplyMenuBackGround();
	void InitialEditorStyle();
	//Dx12不支持播放视频，暂时
	void ApplyEditorBGWithDx12 (TSharedPtr<FString> Item);
	void ApplyPanelBGWithDx12 (TSharedPtr<FString> Item);
	void ApplyThemeStyle();
	void Reimport(const TArray<struct FFileChangeData>& FileChanges);
	void RegisterMenus();


private:
	bool IsEditorVideoMatarial = false;
	bool IsPanelVideoMatarial = false;
	bool bLasyType;
	int LastWallpaperNum = 0;
	UEditorUtilityWidgetBlueprint* EditorWidget;
	FAssetData Data;
	TSharedPtr<class FUICommandList> PluginCommands;
	bool bWatcherIsRunning = false;
	int MaxNum = INDEX_NONE;
	FGuid LastUID;
	FTimerHandle handle;
	FDelegateHandle DirectoryWatcherHandle;
	
	void SetSetting();
	
	UPROPERTY()
	void CreateWatcher();

public:
	FText GetEditorComboBoxContent() const;
	FText GetPanelComboBoxContent() const;
	TSharedRef<SWidget> HandleGenerateWidget(TSharedPtr<FString> Item) const;
	int FindWallpaperIndex(TSharedPtr<FString> Item);
	void HandleEditorSelectionChanged(TSharedPtr<FString> Item);
	void HandlePanelSelectionChanged(TSharedPtr<FString> Item);
	TSharedPtr<FString> CurrentEditorBG;
	TSharedPtr<FString> CurrentPanelBG;
	void ImportWallpaper();
	void ImportPicTheme();
	
	UPROPERTY()
	void CheckTimer();

private:
	FString LastWallpaperPath;
	UPROPERTY()
	UWallPaperBrush* WallpaperPlayer;
	
	TArray<TSharedPtr<FString>> Wallpaperlist;
	
	TArray<TSharedPtr<FString>> WallpaperPath;
	
	TSharedPtr<class SComboBox<TSharedPtr<FString>>> WallPaperPanelComboBox;
	TSharedPtr<SBox> ComboBoxContent;
};
