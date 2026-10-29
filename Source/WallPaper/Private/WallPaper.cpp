// Copyright 2023 Natsu Neko, Inc. All Rights Reserved.

#include "WallPaper.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor.h"
#include "Engine/Texture.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/Commands.h"
#include "Framework/Commands/UICommandList.h"
#include "HAL/FileManager.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "LevelEditor.h"
#include "Materials/Material.h"
#include "Misc/Paths.h"
#include "Settings/EditorStyleSettings.h"
#include "Sources/WallpaperList.h"
#include "Sources/WallpaperSource.h"
#include "Styling/AppStyle.h"
#include "Styling/WallpaperStyleApplier.h"
#include "ToolMenus.h"
#include "WallPaperEditor.h"
#include "WallPaperSetting.h"
#include "Watcher/WallpaperDirectoryWatcher.h"
#include "Widgets/Input/SButton.h"

#define LOCTEXT_NAMESPACE "FWallPaperModule"

DEFINE_LOG_CATEGORY_STATIC(LogWallpaper, Log, All);

namespace
{
	const FName WallpaperToolbarName(TEXT("LevelEditor.LevelEditorToolBar.PlayToolBar"));
	constexpr float RefreshDelaySeconds = 0.25f;

	bool IsWallpaperAssetPath(const FString& Path)
	{
		return Path == TEXT("/Game/Wallpaper") || Path.StartsWith(TEXT("/Game/Wallpaper/"));
	}

	FString NormalizeVideoPath(const FString& Path)
	{
		FString FullPath = FPaths::ConvertRelativePathToFull(Path);
		FPaths::MakeStandardFilename(FullPath);
		return FullPath.ToLower();
	}

	class FWallpaperCommands : public TCommands<FWallpaperCommands>
	{
	public:
		FWallpaperCommands()
			: TCommands<FWallpaperCommands>(TEXT("WallpaperUICommand"), LOCTEXT("WallpaperCommands", "Wallpaper"), NAME_None, FAppStyle::GetAppStyleSetName())
		{
		}

		virtual void RegisterCommands() override
		{
			// Keep the command identifier so existing user key bindings remain valid.
			UI_COMMAND(Excution, "Random Wallpaper", "Choose random wallpapers", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Shift | EModifierKey::Control, EKeys::C));
		}

		TSharedPtr<FUICommandInfo> Excution;
	};
}

FWallPaperModule::FWallPaperModule() = default;
FWallPaperModule::~FWallPaperModule() = default;

void FWallPaperModule::StartupModule()
{
	if (IsRunningCommandlet() || !GEditor || !FSlateApplication::IsInitialized())
	{
		return;
	}

	bStarted = true;
	EditorStyleSettings = GetMutableDefault<UEditorStyleSettings>();
	bOriginalUseGrid = EditorStyleSettings->bUseGrid;
	EditorTimerManager = GEditor->GetTimerManager();
	StyleApplier = MakeUnique<FWallpaperStyleApplier>();
	StyleApplier->CaptureOriginalStyle();
	Settings = GetMutableDefault<UWallPaperSetting>();
	WallpaperPlayer.Reset(NewObject<UWallPaperBrush>(GetTransientPackage(), NAME_None, RF_Transient));
	Wallpapers = MakeUnique<FWallpaperList>();

	RegisterSettings();
	RegisterAssetEvents();
	// Create only the user's input directory, never a duplicate asset cache.
	IFileManager::Get().MakeDirectory(*(FPaths::ProjectContentDir() / TEXT("Wallpaper")), true);
	RefreshWallpapers();
	OnSettingsModified();

	FWallpaperCommands::Register();
	GlobalCommands = FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor")).GetGlobalLevelEditorActions();
	GlobalCommands->MapAction(FWallpaperCommands::Get().Excution, FExecuteAction::CreateRaw(this, &FWallPaperModule::ChangeRandomWallpaper));
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FWallPaperModule::RegisterMenus));
}

void FWallPaperModule::ShutdownModule()
{
	if (!bStarted)
	{
		return;
	}
	bStarted = false;
	FTSTicker::RemoveTicker(RefreshTicker);
	RefreshTicker.Reset();
	if (TSharedPtr<FTimerManager> TimerManager = EditorTimerManager.Pin())
	{
		TimerManager->ClearTimer(WallpaperTimer);
	}
	EditorTimerManager.Reset();
	FWallpaperDirectoryWatcher::Stop(DirectoryWatcherHandle, WatchedDirectory);
	UnregisterAssetEvents();

	if (SettingsSection)
	{
		SettingsSection->OnModified().Unbind();
		SettingsSection->OnSaveDefaults().Unbind();
		SettingsSection->OnResetDefaults().Unbind();
		SettingsSection.Reset();
	}
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings")))
	{
		SettingsModule->UnregisterSettings(TEXT("Editor"), TEXT("General"), TEXT("WallPaper"));
	}
	if (GlobalCommands && FWallpaperCommands::IsRegistered())
	{
		GlobalCommands->UnmapAction(FWallpaperCommands::Get().Excution);
	}
	GlobalCommands.Reset();
	FWallpaperCommands::Unregister();

	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	if (FSlateApplication::IsInitialized() && !IsEngineExitRequested())
	{
		UToolMenus::Get()->RefreshMenuWidget(WallpaperToolbarName);
	}

	StyleApplier->RestoreOriginalStyle();
	if (EditorStyleSettings.IsValid())
	{
		EditorStyleSettings->bUseGrid = bOriginalUseGrid;
	}
	EditorStyleSettings.Reset();
	WallpaperPlayer->StopAll();
	WallpaperPlayer.Reset();
	StyleApplier.Reset();
	Wallpapers.Reset();
	Settings = nullptr;
}

void FWallPaperModule::RegisterSettings()
{
	SettingsSection = FModuleManager::LoadModuleChecked<ISettingsModule>(TEXT("Settings"))
						  .RegisterSettings(TEXT("Editor"), TEXT("Plugins"),TEXT("WallPaper"), LOCTEXT("SettingsName", "Wallpaper"), LOCTEXT("SettingsDescription", "Customize editor wallpapers and colors."), Settings);
	if (!SettingsSection)
	{
		return;
	}
	SettingsSection->OnModified().BindRaw(this, &FWallPaperModule::OnSettingsModified);
	SettingsSection->OnSaveDefaults().BindLambda(
		[this]()
		{
			Settings->UpdateGlobalUserConfigFile();
			return true;
		});
	SettingsSection->OnResetDefaults().BindRaw(this, &FWallPaperModule::ResetSettings);
}

bool FWallPaperModule::OnSettingsModified()
{
	Settings->EditorOpacity = FMath::Clamp(Settings->EditorOpacity, 0.0f, 1.0f);
	Settings->PanelOpacity = FMath::Clamp(Settings->PanelOpacity, 0.0f, 1.0f);
	Settings->Time = FMath::Max(Settings->Time, 1.0f);
	if (bLastUseVideo != Settings->UseWallpaperEngine || LastSourcePath != Settings->WallPaperDirectoryPath.Path)
	{
		RefreshWallpapers();
	}
	EditorStyleSettings->bUseGrid = Settings->EditorUseGrid;
	StyleApplier->ApplyMenuBackground(Settings->Menu_Background);
	StyleApplier->ApplyThemeColors(Settings->StyleColors);
	StyleApplier->ApplyWindowOpacity(Settings->EditorOpacity, Settings->PanelOpacity);
	UpdateTimer();
	return true;
}

bool FWallPaperModule::ResetSettings()
{
	Settings->ResetToDefaults();
	Settings->EditorUseGrid = bOriginalUseGrid;
	OnSettingsModified();
	Settings->SaveConfig();
	Settings->UpdateGlobalUserConfigFile();
	return true;
}

void FWallPaperModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);
	const auto RandomClicked = [this]()
	{
		ChangeRandomWallpaper();
		return FReply::Handled();
	};
	const TSharedRef<SWidget> RandomButton = SNew(SButton).Text(LOCTEXT("RandomWallpaper", "Random")).OnClicked_Lambda(RandomClicked);
	UToolMenu* Toolbar = UToolMenus::Get()->ExtendMenu(WallpaperToolbarName);
	Toolbar->FindOrAddSection(TEXT("PluginTools")).AddEntry(FToolMenuEntry::InitWidget(TEXT("Wallpaper"), RandomButton, LOCTEXT("WallpaperToolbar", "Wallpaper")));
}

void FWallPaperModule::ChangeRandomWallpaper()
{
	for (EWallpaperTarget Target : {EWallpaperTarget::Editor, EWallpaperTarget::Panel})
	{
		Wallpapers->SelectRandom(Target);
		ApplySelection(Target);
	}
}

void FWallPaperModule::ApplySelection(EWallpaperTarget Target)
{
	const TSharedPtr<FWallpaperItem> Item = Wallpapers->GetSelection(Target);
	if (!Item)
	{
		WallpaperPlayer->Stop(Target);
		return;
	}

	UObject* Resource = nullptr;
	if (Item->bIsVideo)
	{
		if (bReloadVideos || ChangedVideoPaths.Contains(NormalizeVideoPath(Item->ResourcePath)))
		{
			WallpaperPlayer->Stop(Target);
		}
		Resource = WallpaperPlayer->Play(Item->ResourcePath, Target);
	}
	else
	{
		WallpaperPlayer->Stop(Target);
		Resource = LoadObject<UTexture>(nullptr, *Item->ResourcePath);
	}
	if (!Resource)
	{
		UE_LOG(LogWallpaper, Warning, TEXT("Could not open wallpaper: %s"), *Item->ResourcePath);
		return;
	}
	const float Opacity = Target == EWallpaperTarget::Editor ? Settings->EditorOpacity : Settings->PanelOpacity;
	StyleApplier->ApplyResource(Resource, Target, Opacity);
}

void FWallPaperModule::RefreshWallpapers()
{
	FTSTicker::RemoveTicker(RefreshTicker);
	RefreshTicker.Reset();
	TArray<FWallpaperItem> Items;
	if (Settings->UseWallpaperEngine)
	{
		Items = FWallpaperSource::ScanWallpaperEngine(Settings->WallPaperDirectoryPath.Path);
		if (Items.IsEmpty())
		{
			UE_LOG(LogWallpaper, Warning, TEXT("No MP4 wallpapers found under %s; using images until videos are available."), *Settings->WallPaperDirectoryPath.Path);
		}
	}
	bUsingProjectTextures = Items.IsEmpty();
	if (bUsingProjectTextures)
	{
		Items = FWallpaperSource::ScanProjectTextures();
	}
	bLastUseVideo = Settings->UseWallpaperEngine;
	LastSourcePath = Settings->WallPaperDirectoryPath.Path;
	Wallpapers->SetItems(MoveTemp(Items));
	ApplySelection(EWallpaperTarget::Editor);
	ApplySelection(EWallpaperTarget::Panel);
	ChangedVideoPaths.Reset();
	bReloadVideos = false;
	UpdateWatcher();
	UpdateTimer();
}

void FWallPaperModule::UpdateTimer()
{
	const TSharedPtr<FTimerManager> TimerManager = EditorTimerManager.Pin();
	if (!TimerManager)
	{
		return;
	}
	if (!Settings->bEnableTimer || Wallpapers->GetItems().Num() < 2)
	{
		TimerManager->ClearTimer(WallpaperTimer);
		return;
	}
	const float Interval = FMath::Max(Settings->Time, 1.0f) * 60.0f;
	if (!TimerManager->IsTimerActive(WallpaperTimer) || !FMath::IsNearlyEqual(TimerManager->GetTimerRate(WallpaperTimer), Interval))
	{
		TimerManager->SetTimer(WallpaperTimer, FTimerDelegate::CreateRaw(this, &FWallPaperModule::ChangeRandomWallpaper), Interval, true);
	}
}

void FWallPaperModule::UpdateWatcher()
{
	if (!Settings->UseWallpaperEngine)
	{
		FWallpaperDirectoryWatcher::Stop(DirectoryWatcherHandle, WatchedDirectory);
		return;
	}
	FWallpaperDirectoryWatcher::Watch(DirectoryWatcherHandle, WatchedDirectory, Settings->WallPaperDirectoryPath.Path / TEXT("steamapps/workshop/content"),
		IDirectoryWatcher::FDirectoryChanged::CreateRaw(this, &FWallPaperModule::OnDirectoryChanged));
}

void FWallPaperModule::QueueWallpaperRefresh()
{
	if (!bStarted)
	{
		return;
	}
	FTSTicker::RemoveTicker(RefreshTicker);
	const FTickerDelegate RefreshDelegate = FTickerDelegate::CreateLambda(
		[this](float)
		{
			RefreshTicker.Reset();
			RefreshWallpapers();
			return false;
		});
	RefreshTicker = FTSTicker::GetCoreTicker().AddTicker(RefreshDelegate, RefreshDelaySeconds);
}

void FWallPaperModule::OnDirectoryChanged(const TArray<FFileChangeData>& FileChanges)
{
	bool bNeedsRefresh = false;
	for (const FFileChangeData& Change : FileChanges)
	{
		const FString Extension = FPaths::GetExtension(Change.Filename);
		if (Extension.Equals(TEXT("mp4"), ESearchCase::IgnoreCase))
		{
			ChangedVideoPaths.Add(NormalizeVideoPath(Change.Filename));
			bNeedsRefresh = true;
		}
		else if (Change.Action == FFileChangeData::FCA_RescanRequired || Change.Action == FFileChangeData::FCA_Removed || Extension.IsEmpty() || IFileManager::Get().DirectoryExists(*Change.Filename))
		{
			// Overflow and directory moves may not identify individual video files.
			bReloadVideos = true;
			bNeedsRefresh = true;
		}
	}
	if (bNeedsRefresh)
	{
		QueueWallpaperRefresh();
	}
}

void FWallPaperModule::OnAssetChanged(const FAssetData& Asset)
{
	if (bUsingProjectTextures && IsWallpaperAssetPath(Asset.PackagePath.ToString()))
	{
		QueueWallpaperRefresh();
	}
}

void FWallPaperModule::OnAssetRenamed(const FAssetData& Asset, const FString& OldObjectPath)
{
	if (bUsingProjectTextures && (IsWallpaperAssetPath(Asset.PackagePath.ToString()) || IsWallpaperAssetPath(OldObjectPath)))
	{
		QueueWallpaperRefresh();
	}
}

void FWallPaperModule::RegisterAssetEvents()
{
	AssetRegistry = &FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
	AssetRegistry->OnAssetAdded().AddRaw(this, &FWallPaperModule::OnAssetChanged);
	AssetRegistry->OnAssetRemoved().AddRaw(this, &FWallPaperModule::OnAssetChanged);
	AssetRegistry->OnAssetRenamed().AddRaw(this, &FWallPaperModule::OnAssetRenamed);
	AssetRegistry->OnAssetUpdated().AddRaw(this, &FWallPaperModule::OnAssetChanged);
	AssetRegistry->OnAssetUpdatedOnDisk().AddRaw(this, &FWallPaperModule::OnAssetChanged);
	AssetRegistry->OnFilesLoaded().AddRaw(this, &FWallPaperModule::QueueWallpaperRefresh);
}

void FWallPaperModule::UnregisterAssetEvents()
{
	if (AssetRegistry && FModuleManager::Get().IsModuleLoaded(TEXT("AssetRegistry")))
	{
		AssetRegistry->OnAssetAdded().RemoveAll(this);
		AssetRegistry->OnAssetRemoved().RemoveAll(this);
		AssetRegistry->OnAssetRenamed().RemoveAll(this);
		AssetRegistry->OnAssetUpdated().RemoveAll(this);
		AssetRegistry->OnAssetUpdatedOnDisk().RemoveAll(this);
		AssetRegistry->OnFilesLoaded().RemoveAll(this);
	}
	AssetRegistry = nullptr;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FWallPaperModule, WallPaper)
