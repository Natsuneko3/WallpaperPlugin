// Copyright 2023 Natsu Neko, Inc. All Rights Reserved.

#include "WallPaper.h"
#include "Interfaces/IPluginManager.h"
#include "DetailLayoutBuilder.h"
#include "EditorUtilitySubsystem.h"
#include "ISettingsSection.h"
#include "WallPaperSetting.h"
#include "Settings/EditorStyleSettings.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Engine/Texture.h"
#include "LevelEditor.h"
#include "LevelEditorActions.h"
#include "HAL/FileManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/MessageDialog.h"
#include "Sources/WallpaperSource.h"
#include "Styling/AppStyle.h"
#include "Styling/WallpaperStyleApplier.h"
#include "Watcher/WallpaperDirectoryWatcher.h"


#define LOCTEXT_NAMESPACE "FWallPaperModule"

class WallpaperCommands:public TCommands<WallpaperCommands>
{
public:
	WallpaperCommands()
		: TCommands<WallpaperCommands>(
			"WallpaperUICommand",
			NSLOCTEXT("Contexts", "WallpaperUICommands", "Wallpaper UI Command"),
			NAME_None, FAppStyle::GetAppStyleSetName()
		)
	{ }

	virtual void RegisterCommands() override
	{
		UI_COMMAND(Excution, "Excution", "Random Wallpaper", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Shift|EModifierKey::Control,EKeys::C));
		//UI_COMMAND( Excution, "Excution", "Random Wallpaper", EUserInterfaceActionType::Button, FInputChord() );
	}
	TSharedPtr<FUICommandInfo> Excution;
	
};

void FWallPaperModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FString AssetPath = "/WallPaper/WallPaperEngine/UMG";
	UPackage* EditorTexturePack = CreatePackage(*AssetPath);
	WallpaperPlayer = NewObject<UWallPaperBrush>(EditorTexturePack, FName("WallpaperPlayer"), RF_MarkAsRootSet);
	WallpaperPlayer->SetCanPlayVideo(StyleSettings->UseWallpaperEngine);
	bLasyType = StyleSettings->UseWallpaperEngine;


	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float Time)
		{
			CheckTimer();
			ChangeRandomWallpaper();
			return false;
		}), 1.f);



	ImportWallpaper();
	GetMutableDefault<UEditorStyleSettings>()->bUseGrid = StyleSettings->EditorUseGrid;
	ApplyMenuBackGround();
	InitialEditorStyle();

	ApplyThemeStyle();
	SetSetting();
	CreateWatcher();

	WallpaperCommands::Register();
	FLevelEditorModule& LevelEditorModule = FModuleManager::Get().LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	TSharedRef<FUICommandList> CommandList = LevelEditorModule.GetGlobalLevelEditorActions();
	//CommandList->MapAction(WallpaperCommands::Get().Excution,FExecuteAction::CreateStatic(&FLevelEditorActionCallbacks::NewLevel));
	CommandList->MapAction(WallpaperCommands::Get().Excution, 
		FExecuteAction::CreateLambda([this]
		{
			ChangeRandomWallpaper();
		}),
		FCanExecuteAction()
	);

	//GEditor->GetTimerManager()->SetTimerForNextTick([this](){CheckTimer();});
	


	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FWallPaperModule::RegisterMenus));
	
}

void FWallPaperModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FWallpaperDirectoryWatcher::Stop(DirectoryWatcherHandle, WatchedDirectory);
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").UnregisterSettings("Editor", "General", "WallPaper");
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	WallpaperCommands::Unregister();
	StyleSettings->ResetStyleColor();
	//Clear Cache
	FString PluginsPath = FPaths::ProjectPluginsDir()/"Wallpaper";
	if(!IFileManager::Get().DirectoryExists(*PluginsPath))
    	{
    		PluginsPath = FPaths::EnginePluginsDir()/"Wallpaper";
    	}
    		
    	FString TargetFilePath = PluginsPath/"Content/Cache";
    	if(IFileManager::Get().DirectoryExists(*TargetFilePath))
    	{
    		IFileManager::Get().DeleteDirectory(*(TargetFilePath),false,true);
    	}
}

bool FWallPaperModule::OnSettingModified()
{
	WallpaperPlayer->SetCanPlayVideo(StyleSettings->UseWallpaperEngine);
	if (bLasyType != StyleSettings->UseWallpaperEngine || LastWallpaperPath != StyleSettings->WallPaperDirectoryPath.
		Path)
	{
		bLasyType = StyleSettings->UseWallpaperEngine;
		TArray<struct FFileChangeData> FileChanges;
		Reimport(FileChanges);
		CreateWatcher();
	}

	GetMutableDefault<UEditorStyleSettings>()->bUseGrid = StyleSettings->EditorUseGrid;
	ApplyMenuBackGround();
	ApplyThemeStyle();
	CheckTimer();

	FWallpaperStyleApplier::ApplyWindowOpacity(StyleSettings->EditorOpacity, StyleSettings->PanelOpacity);
	return true;
}


void FWallPaperModule::PluginButtonClicked()
{
	EditorWidget = LoadObject<UEditorUtilityWidgetBlueprint>(
		NULL,TEXT("/WallPaper/WallPaperEngine/UMG/WallPaperEditor"));

	if (EditorWidget)
	{
		UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
		EditorUtilitySubsystem->SpawnAndRegisterTab(EditorWidget);
	}
}


void FWallPaperModule::InitialEditorStyle()
{
	FWallpaperStyleApplier::MakeGraphPanelTransparent();

	if (IsValid(WallpaperPlayer))
	{
		if (WallpaperPlayer->CanPlayvideo())
		{
			if (Wallpaperlist.Num() == 0)
			{
				UTexture* EditorTexture = LoadObject<UTexture>(
					NULL,TEXT("/WallPaper/WallPaperEngine/Backgound/Default_1"));
				UTexture* PanelTexture = LoadObject<UTexture>(
					NULL,TEXT("/WallPaper/WallPaperEngine/Backgound/Default_2"));
				FWallpaperStyleApplier::ApplyEditorTexture(EditorTexture, StyleSettings->EditorOpacity);
				FWallpaperStyleApplier::ApplyPanelTexture(PanelTexture, StyleSettings->PanelOpacity);
			}
			else if(LastWallpaperNum != Wallpaperlist.Num())
			{
				LastWallpaperNum = Wallpaperlist.Num();
				int RamdomEditor = FMath::RandRange(0, FMath::Max(Wallpaperlist.Num() - 2, 0));
				int RandomPanel = FMath::RandRange(0, Wallpaperlist.Num() - 1);
				HandleEditorSelectionChanged(Wallpaperlist[RamdomEditor]);
				HandlePanelSelectionChanged(Wallpaperlist[RandomPanel]);
			}
		}
		else
		{
			if (Wallpaperlist.Num() > 0&&LastWallpaperNum != Wallpaperlist.Num())
			{
				LastWallpaperNum = Wallpaperlist.Num();
				int RamdomEditor = FMath::RandRange(0, FMath::Max(Wallpaperlist.Num() - 2, 0));
				int RandomPanel = FMath::RandRange(0, Wallpaperlist.Num() - 1);
				ApplyEditorBGWithDx12(Wallpaperlist[RamdomEditor]);
				ApplyPanelBGWithDx12(Wallpaperlist[RandomPanel]);
			}
		}
	}
}

void FWallPaperModule::ApplyMenuBackGround()
{
	FWallpaperStyleApplier::ApplyMenuBackground(StyleSettings->Menu_Background);
}

void FWallPaperModule::ApplyEditorBGWithDx12(TSharedPtr<FString> Item)
{
	int index = FindWallpaperIndex(Item);
	if (!WallpaperPath.IsValidIndex(index))
	{
		return;
	}

	const FString FilePath = *WallpaperPath[index];
	UTexture* Texture = LoadObject<UTexture>(NULL, *FilePath);
	FWallpaperStyleApplier::ApplyEditorTexture(Texture, StyleSettings->EditorOpacity);
}

void FWallPaperModule::ApplyPanelBGWithDx12(TSharedPtr<FString> Item)
{
	int index = FindWallpaperIndex(Item);
	if (!WallpaperPath.IsValidIndex(index))
	{
		return;
	}

	const FString FilePath = *WallpaperPath[index];
	UTexture* Texture = LoadObject<UTexture>(NULL, *FilePath);
	FWallpaperStyleApplier::ApplyPanelTexture(Texture, StyleSettings->PanelOpacity);
}


void FWallPaperModule::ApplyThemeStyle()
{
	FWallpaperStyleApplier::ApplyThemeColors(StyleSettings->StyleColors, LastUID);
}


void FWallPaperModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);
	/*FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>( TEXT("LevelEditor") );
	TSharedRef<  FUICommandList > CommandBindings = LevelEditorModule.GetGlobalLevelEditorActions();
	
	
	CommandBindings->MapAction(WallpaperCommands::Get().Excution,
		FExecuteAction::CreateStatic(&FWallPaperModule::ImportPicTheme),
		FCanExecuteAction()
		);*/
	
	


	TSharedRef<SWidget> WallpaperWidget = SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		  .AutoWidth()
		  .VAlign(VAlign_Center)
		[
			SNew(SCheckBox)
		.ToolTipText(LOCTEXT("CheckBoxTooltip", "Enable Wallpaper engine"))
		.IsChecked_Lambda(
				               [this]() -> ECheckBoxState
				               {
					               return StyleSettings->UseWallpaperEngine
						                      ? ECheckBoxState::Checked
						                      : ECheckBoxState::Unchecked;
				               })
		.OnCheckStateChanged_Lambda([this](ECheckBoxState InCheckBoxState)
			               {
				               if (InCheckBoxState == ECheckBoxState::Checked)
				               {
					               StyleSettings->UseWallpaperEngine = true;
				               }
				               else
				               {
					               StyleSettings->UseWallpaperEngine = false;
				               }
				               WallpaperPlayer->SetCanPlayVideo(StyleSettings->UseWallpaperEngine);
				               TArray<struct FFileChangeData> FileChanges;
				               Reimport(FileChanges);
								CreateWatcher();
			               })
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SAssignNew(WallPaperEditorComboBox, SComboBox< TSharedPtr<FString> >)
			.OptionsSource(&Wallpaperlist)

			//.InitiallySelectedItem(Wallpaperlist[10])
			.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
			{
				return SNew(STextBlock)
					.Text(FText::FromString(*Item.Get()));
			})
			.OnSelectionChanged_Lambda([this](TSharedPtr<FString> Item, ESelectInfo::Type SelectionType)
			{
				HandleEditorSelectionChanged(Item);
			})
			.ComboBoxStyle(FAppStyle::Get(), "SimpleComboBox")
			.Content()
			[
				SNew(STextBlock)
				.Text_Raw(this, &FWallPaperModule::GetEditorComboBoxContent)
			]

		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SAssignNew(WallPaperPanelComboBox, SComboBox< TSharedPtr<FString> >)
			.OptionsSource(&Wallpaperlist)

			.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
			{
				return SNew(STextBlock)
					.Text(FText::FromString(*Item.Get()));
			})
			.OnSelectionChanged_Lambda([this](TSharedPtr<FString> Item, ESelectInfo::Type SelectionType)
			{
				HandlePanelSelectionChanged(Item);
			})
			.ComboBoxStyle(FAppStyle::Get(), "SimpleComboBox")
			.Content()
			[
				SNew(STextBlock)
				.Text_Raw(this, &FWallPaperModule::GetPanelComboBoxContent)

			]
			]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			 .OnClicked_Lambda([this]()
			 {
				 ChangeRandomWallpaper();
				 return FReply::Handled();
			 })
			.Text(LOCTEXT("RandomWallpaper", "Random"))

		];



	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				//ToolbarMenu->re
				FToolMenuEntry& Entry = Section.
					AddEntry(FToolMenuEntry::InitWidget("Wallpaper", WallpaperWidget,
					                                    LOCTEXT("Wallpaper", "WallpaperEditor")));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

void FWallPaperModule::ChangeRandomWallpaper()
{
	UE_LOG(LogTemp,Log,TEXT("Change"));
	if(Wallpaperlist.Num()>2)
	{
		if(WallpaperPlayer->CanPlayvideo())
		{
			int RamdomEditor = FMath::RandRange(0, FMath::Max(Wallpaperlist.Num() - 2, 0));
			int RandomPanel = FMath::RandRange(0, Wallpaperlist.Num() - 1);
			HandleEditorSelectionChanged(Wallpaperlist[RamdomEditor]);
			HandlePanelSelectionChanged(Wallpaperlist[RandomPanel]);
		}
		else
		{
			int RamdomEditor = FMath::RandRange(0, FMath::Max(Wallpaperlist.Num() - 2, 0));
			int RandomPanel = FMath::RandRange(0, Wallpaperlist.Num() - 1);
			ApplyEditorBGWithDx12(Wallpaperlist[RamdomEditor]);
			ApplyPanelBGWithDx12(Wallpaperlist[RandomPanel]);
		}
	}
}


void FWallPaperModule::SetSetting()
{
	SettingS = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").RegisterSettings("Editor", "General",
		TEXT("WallPaper"),
		FText::FromString("WallPaper"),
		FText::FromString("Setting For Wallpaper"),
		GetMutableDefault<UWallPaperSetting>());
	SettingS->OnModified().BindRaw(this, &FWallPaperModule::OnSettingModified);
	SettingS->OnSaveDefaults().BindLambda([=]()-> bool
	{
		GetMutableDefault<UWallPaperSetting>()->UpdateGlobalUserConfigFile();
		return true;
	});
	SettingS->OnResetDefaults().BindLambda([this]()-> bool
	{
		StyleSettings->ResetStyleColor();
		StyleSettings->InitCoreStyle();
		InitialEditorStyle();
		ApplyThemeStyle();
		TArray<FString> ConfigNeedToReset;
		FString ConfigSectionName = GetMutableDefault<UWallPaperSetting>()->GetClass()->GetPathName();
		ConfigNeedToReset.Add(GetMutableDefault<UWallPaperSetting>()->GetClass()->GetConfigName());
		ConfigNeedToReset.Add(GetMutableDefault<UWallPaperSetting>()->GetGlobalUserConfigFilename());
		//ConfigNeedToReset.Add(GetMutableDefault<UWallPaperSetting>()->Get);
		for (FString ConfigFileName : ConfigNeedToReset)
		{
			//GConfig->GetString(*ConfigSectionName,)
			GConfig->EmptySection(*ConfigSectionName, ConfigFileName);
			GConfig->Flush(false);
		}
		FConfigCacheIni::LoadGlobalIniFile(ConfigNeedToReset[0], *FPaths::GetBaseFilename(ConfigNeedToReset[0]),
		                                   nullptr, true);
		GetMutableDefault<UWallPaperSetting>()->ReloadConfig(nullptr, nullptr,
		                                                     UE::LCPF_PropagateToInstances |
		                                                     UE::LCPF_PropagateToChildDefaultObjects);
		//FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ConfigReset_RestartEditor", "Config has been reset.Restart Editor to take effect."));


		return true;
	});
}



void FWallPaperModule::CheckTimer()
{
	
	FTimerManager& timerManager = GEditor->GetTimerManager().Get();
	if (handle.IsValid())
	{
		timerManager.ClearTimer(handle);
	}
	//FTimerManager& timerManager = GEditorIni.end()
	if (StyleSettings->bEnableTimer && Wallpaperlist.Num() > 0)
	{
		if (handle.IsValid())
		{
			timerManager.ClearTimer(handle);
		}
		auto lambda = [this]()
		{
			ChangeRandomWallpaper();
		};
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda(lambda);
		float TImerDuration = FMath::Max(StyleSettings->Time, 1.0f);
		timerManager.SetTimer(handle, TimerDelegate, TImerDuration * 60, true);
	}
}

void FWallPaperModule::CreateWatcher()
{
	if (Wallpaperlist.Num() > 0)
	{
		if (WallpaperPlayer->CanPlayvideo())
		{
			const FString Path = (StyleSettings->WallPaperDirectoryPath.Path) + "/steamapps/workshop/content";
			FWallpaperDirectoryWatcher::Watch(
				DirectoryWatcherHandle,
				WatchedDirectory,
				Path,
				IDirectoryWatcher::FDirectoryChanged::CreateRaw(this, &FWallPaperModule::Reimport));
		}
		else
		{
			const FString FilePath = FPaths::ProjectContentDir() / "Wallpaper";
			FWallpaperDirectoryWatcher::Watch(
				DirectoryWatcherHandle,
				WatchedDirectory,
				FilePath,
				IDirectoryWatcher::FDirectoryChanged::CreateRaw(this, &FWallPaperModule::Reimport));
		}
	}
	else
	{
		FWallpaperDirectoryWatcher::Stop(DirectoryWatcherHandle, WatchedDirectory);
	}
}

FText FWallPaperModule::GetEditorComboBoxContent() const
{
	FString OutText = "Editor";
	return FText::FromString(OutText);
}

void FWallPaperModule::Reimport(const TArray<struct FFileChangeData>& FileChanges)
{
	UE_LOG(LogTemp, Log, TEXT("FIle is reimport"));
	ImportWallpaper();
	InitialEditorStyle();
}


FText FWallPaperModule::GetPanelComboBoxContent() const
{
	FString OutText = "Panel"; //
	return FText::FromString(OutText);
}

void FWallPaperModule::ImportWallpaper()
{
	WallpaperPath.Reset();
	Wallpaperlist.Reset();
	LastWallpaperPath = StyleSettings->WallPaperDirectoryPath.Path;

	TArray<FWallpaperItem> Items;
	if (WallpaperPlayer->CanPlayvideo())
	{
		Items = FWallpaperSource::ScanWallpaperEngine(StyleSettings->WallPaperDirectoryPath.Path);
		if (Items.Num() == 0)
		{
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("WallpaperTip",
			                                              "Your wallpaper engine path is no vaild.\n please go to editor setting -> wallpaper check you setting"));
			StyleSettings->UseWallpaperEngine = false;
			WallpaperPlayer->SetCanPlayVideo(StyleSettings->UseWallpaperEngine);
			bool bFoundProjectTextures = false;
			Items = FWallpaperSource::ScanProjectTextures(bFoundProjectTextures);
			if (!bFoundProjectTextures)
			{
				LastWallpaperNum = -1;
			}
		}
	}
	else
	{
		StyleSettings->UseWallpaperEngine = false;
		WallpaperPlayer->SetCanPlayVideo(StyleSettings->UseWallpaperEngine);
		bool bFoundProjectTextures = false;
		Items = FWallpaperSource::ScanProjectTextures(bFoundProjectTextures);
		if (!bFoundProjectTextures)
		{
			LastWallpaperNum = -1;
		}
	}

	MaxNum = Items.Num();
	for (const FWallpaperItem& Item : Items)
	{
		Wallpaperlist.Add(MakeShared<FString>(Item.DisplayName));
		WallpaperPath.Add(MakeShared<FString>(Item.ResourcePath));
	}

	UToolMenus::Get()->RefreshAllWidgets();
}

void FWallPaperModule::ImportPicTheme()
{
	WallpaperPath.Reset();
	Wallpaperlist.Reset();

	bool bFoundProjectTextures = false;
	const TArray<FWallpaperItem> Items = FWallpaperSource::ScanProjectTextures(bFoundProjectTextures);
	if (!bFoundProjectTextures)
	{
		LastWallpaperNum = -1;
	}

	MaxNum = Items.Num();
	for (const FWallpaperItem& Item : Items)
	{
		Wallpaperlist.Add(MakeShared<FString>(Item.DisplayName));
		WallpaperPath.Add(MakeShared<FString>(Item.ResourcePath));
	}
}


TSharedRef<SWidget> FWallPaperModule::HandleGenerateWidget(TSharedPtr<FString> Item) const
{
	FString StringItem = Item.IsValid() ? *Item : FString();

	return SNew(STextBlock)
		.Text(FText::FromString(StringItem)); //.Margin(FMargin(0.0,15.0,0.0,0.0))
}

int FWallPaperModule::FindWallpaperIndex(TSharedPtr<FString> Item)
{
	if (!Item.IsValid() || Wallpaperlist.Num() == 0)
	{
		return INDEX_NONE;
	}

	int Num = FMath::RandRange(0, Wallpaperlist.Num() - 1);
	//找到并且播放
	for (int32 OptionIndex = 0; OptionIndex < Wallpaperlist.Num(); ++OptionIndex)
	{
		if (Wallpaperlist[OptionIndex] == Item || (Wallpaperlist[OptionIndex].IsValid() && *Wallpaperlist[OptionIndex] == *Item))
		{
			Num = OptionIndex;
			break;
		}
	}
	return Num;
}

void FWallPaperModule::ApplyWallpaperToTarget(TSharedPtr<FString> Item, EWallpaperTarget Target)
{
	if (!IsValid(WallpaperPlayer) || !Item.IsValid())
	{
		return;
	}

	const int WallpaperIndex = FindWallpaperIndex(Item);
	if (!WallpaperPath.IsValidIndex(WallpaperIndex))
	{
		return;
	}

	if (!WallpaperPlayer->CanPlayvideo())
	{
		if (Target == EWallpaperTarget::Editor)
		{
			ApplyEditorBGWithDx12(Item);
		}
		else
		{
			ApplyPanelBGWithDx12(Item);
		}
		return;
	}

	if (Target == EWallpaperTarget::Editor)
	{
		FWallpaperStyleApplier::ApplyEditorMaterial(WallpaperPlayer->GetEditorMaterial(), StyleSettings->EditorOpacity);
		IsEditorVideoMatarial = true;
		WallpaperPlayer->EditorPlay(*WallpaperPath[WallpaperIndex]);
	}
	else
	{
		FWallpaperStyleApplier::ApplyPanelMaterial(WallpaperPlayer->GetPanelMaterial(), StyleSettings->PanelOpacity);
		IsPanelVideoMatarial = true;
		WallpaperPlayer->PanelPlay(*WallpaperPath[WallpaperIndex]);
	}
}


void FWallPaperModule::HandleEditorSelectionChanged(TSharedPtr<FString> Item)
{
	if (IsValid(WallpaperPlayer))
	{
		ApplyWallpaperToTarget(Item, EWallpaperTarget::Editor);

		//随机主题颜色
		if (StyleSettings->RandomMainThemeColor)
		{
			FLinearColor RandomColor = FLinearColor::MakeFromHSV8(FMath::Rand() * 360, 0.6 * 255, 0.1 * 255);
			RandomColor.A = 0.2;
			StyleSettings->StyleColors.StyleColors[(int)EStyleColor::Recessed] = RandomColor;
			ApplyThemeStyle();
		}
	}
}

void FWallPaperModule::HandlePanelSelectionChanged(TSharedPtr<FString> Item)
{
	if (IsValid(WallpaperPlayer))
	{
		ApplyWallpaperToTarget(Item, EWallpaperTarget::Panel);
	}
	//UE_LOG(LogTemp, Warning, TEXT("WallpaperPlayer is no valid"))
}
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FWallPaperModule, WallPaper)
