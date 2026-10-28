// Copyright 2023 Natsu Neko, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "Modules/ModuleManager.h"
#include "TimerManager.h"
#include "UObject/StrongObjectPtr.h"
#include "WallpaperTarget.h"

class FUICommandList;
class FWallpaperList;
class FWallpaperStyleApplier;
class IAssetRegistry;
class ISettingsSection;
class UWallPaperBrush;
class UWallPaperSetting;
class UEditorStyleSettings;
struct FAssetData;
struct FFileChangeData;

class FWallPaperModule : public IModuleInterface
{
public:
	FWallPaperModule();
	virtual ~FWallPaperModule() override;
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterSettings();
	bool OnSettingsModified();
	bool ResetSettings();
	void RegisterMenus();
	void ChangeRandomWallpaper();
	void ApplySelection(EWallpaperTarget Target);
	void RefreshWallpapers();
	void UpdateTimer();
	void UpdateWatcher();
	void QueueWallpaperRefresh();
	void OnDirectoryChanged(const TArray<FFileChangeData>& FileChanges);
	void OnAssetChanged(const FAssetData& Asset);
	void OnAssetRenamed(const FAssetData& Asset, const FString& OldObjectPath);
	void RegisterAssetEvents();
	void UnregisterAssetEvents();

	UWallPaperSetting* Settings = nullptr;
	TWeakObjectPtr<UEditorStyleSettings> EditorStyleSettings;
	TStrongObjectPtr<UWallPaperBrush> WallpaperPlayer;
	TUniquePtr<FWallpaperStyleApplier> StyleApplier;
	TUniquePtr<FWallpaperList> Wallpapers;
	TSharedPtr<ISettingsSection> SettingsSection;
	TSharedPtr<FUICommandList> GlobalCommands;
	IAssetRegistry* AssetRegistry = nullptr;
	FTimerHandle WallpaperTimer;
	TWeakPtr<FTimerManager> EditorTimerManager;
	FTSTicker::FDelegateHandle RefreshTicker;
	FDelegateHandle DirectoryWatcherHandle;
	FString WatchedDirectory;
	FString LastSourcePath;
	TSet<FString> ChangedVideoPaths;
	bool bLastUseVideo = false;
	bool bUsingProjectTextures = true;
	bool bReloadVideos = false;
	bool bOriginalUseGrid = false;
	bool bStarted = false;
};
