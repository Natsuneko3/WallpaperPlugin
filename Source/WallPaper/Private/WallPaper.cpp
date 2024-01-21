// Copyright 2023 Natsu Neko, Inc. All Rights Reserved.

#include "WallPaper.h"

#include "DetailLayoutBuilder.h"
#include "DirectoryWatcherModule.h"
#include "EditorUtilitySubsystem.h"
#include "ISettingsSection.h"
#include "WallPaperSetting.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Settings/EditorStyleSettings.h"


#include "IBlutilityModule.h"
#include "IContentBrowserDataModule.h"
#include "IDirectoryWatcher.h"

#include "Editor.h"
#include "EditorStyleSet.h"
#include "Editor/EditorEngine.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructure.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"

#include "LevelEditor.h"
#include "LevelEditorActions.h"
#include "RenderGraphResources.h"


#include "Styling/SlateStyle.h"
#include "Engine/Blueprint.h"


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

	auto& CoreStyles = FCoreStyle::Get();
	FWindowStyle& WindowsEditor = (FWindowStyle&)CoreStyles.GetWidgetStyle<FWindowStyle>("Window");
	WindowsEditor.BackgroundBrush.TintColor = FLinearColor(FVector(StyleSettings->EditorOpacity));
	WindowsEditor.ChildBackgroundBrush.TintColor = FLinearColor(FVector(StyleSettings->PanelOpacity));
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
	auto& EditorStyles =  FAppStyle::Get();
	FSlateBrush& SolidBackground = *(FSlateBrush*)EditorStyles.GetBrush("Graph.Panel.SolidBackground");

	auto& CoreStyles = FCoreStyle::Get();
	FWindowStyle& WindowsEditor = (FWindowStyle&)CoreStyles.GetWidgetStyle<FWindowStyle>("Window");
	SolidBackground.TintColor = FLinearColor(0, 0, 0, 0);

	if (IsValid(WallpaperPlayer))
	{
		//设置主界面与材质界面图，通过material
		FSlateBrush Editor;
		FSlateBrush panel;
		FSlateBrush OpacityZero;
		if (WallpaperPlayer->CanPlayvideo())
		{
			if (Wallpaperlist.Num() == 0)
			{
				UTexture* EditorTexture = LoadObject<UTexture>(
					NULL,TEXT("/WallPaper/WallPaperEngine/Backgound/Default_1"));
				UTexture* PanelTexture = LoadObject<UTexture>(
					NULL,TEXT("/WallPaper/WallPaperEngine/Backgound/Default_2"));
				Editor.SetResourceObject(EditorTexture);
				Editor.SetImageSize(FVector2D(32, 32));

				panel.SetResourceObject(PanelTexture);
				panel.SetImageSize(FVector2D(32, 32));

				WindowsEditor.BackgroundBrush = Editor;
				WindowsEditor.BackgroundBrush.TintColor = FLinearColor(FVector(StyleSettings->EditorOpacity));
				WindowsEditor.ChildBackgroundBrush = panel;
				WindowsEditor.ChildBackgroundBrush.TintColor = FLinearColor(FVector(StyleSettings->PanelOpacity));
			}
			else if(LastWallpaperNum != Wallpaperlist.Num())
			{
				LastWallpaperNum = Wallpaperlist.Num();
				int RamdomEditor = FMath::Max(FMath::RandRange(0, Wallpaperlist.Num() - 2), 0);
				int RandomPanel = FMath::Max(FMath::RandRange(0, Wallpaperlist.Num() - 1), 0);
				HandleEditorSelectionChanged(Wallpaperlist[RamdomEditor]);
				HandlePanelSelectionChanged(Wallpaperlist[RandomPanel]);
			}
		}
		else
		{
			if (Wallpaperlist.Num() > 0&&LastWallpaperNum != Wallpaperlist.Num())
			{
				LastWallpaperNum = Wallpaperlist.Num();
				int RamdomEditor = FMath::Max(FMath::RandRange(0, Wallpaperlist.Num() - 2), 0);
				int RandomPanel = FMath::Max(FMath::RandRange(0, Wallpaperlist.Num() - 1), 0);
				ApplyEditorBGWithDx12(Wallpaperlist[RamdomEditor]);
				ApplyPanelBGWithDx12(Wallpaperlist[RandomPanel]);
			}
		}
	}
}

void FWallPaperModule::ApplyMenuBackGround()
{
	auto& CoreStyles = FCoreStyle::Get();
	FWindowStyle& WindowsEditor = (FWindowStyle&)CoreStyles.GetWidgetStyle<FWindowStyle>("Window");
	FSlateBrush& MenuBackground = *(FSlateBrush*)CoreStyles.GetBrush("Menu.Background");
	StyleSettings->Menu_Background.ImageSize = FVector2D(32, 32);
	MenuBackground = StyleSettings->Menu_Background;
}

void FWallPaperModule::ApplyEditorBGWithDx12(TSharedPtr<FString> Item)
{
	FSlateBrush Editor;
	int index = FindWallpaperIndex(Item);
	FString FilePath = *WallpaperPath[index];
	UTexture* Texture = LoadObject<UTexture>(NULL, *FilePath);
	if (Texture)
	{
		Editor.SetResourceObject(Texture);
		Editor.SetImageSize(FVector2D(32, 32));
	}


	auto& CoreStyles = FCoreStyle::Get();
	FWindowStyle& WindowsEditor = (FWindowStyle&)CoreStyles.GetWidgetStyle<FWindowStyle>("Window");
	WindowsEditor.BackgroundBrush = Editor;
	WindowsEditor.BackgroundBrush.TintColor = FLinearColor(FVector(StyleSettings->EditorOpacity));
}

void FWallPaperModule::ApplyPanelBGWithDx12(TSharedPtr<FString> Item)
{
	FSlateBrush panel;
	int index = FindWallpaperIndex(Item);
	FString FilePath = *WallpaperPath[index];
	UTexture* Texture = LoadObject<UTexture>(NULL, *FilePath);
	if (Texture)
	{
		panel.SetResourceObject(Texture);
		panel.SetImageSize(FVector2D(32, 32));
	}


	auto& CoreStyles = FCoreStyle::Get();
	FWindowStyle& WindowsEditor = (FWindowStyle&)CoreStyles.GetWidgetStyle<FWindowStyle>("Window");
	WindowsEditor.ChildBackgroundBrush = panel;
	WindowsEditor.ChildBackgroundBrush.TintColor = FLinearColor(FVector(StyleSettings->PanelOpacity));
	auto& EditorStyles = FAppStyle::Get();
	FSlateBrush& SolidBackground = *(FSlateBrush*)EditorStyles.GetBrush("Graph.Panel.SolidBackground");
	SolidBackground.TintColor = FLinearColor(0, 0, 0, 0);
}


void FWallPaperModule::ApplyThemeStyle()
{
	USlateThemeManager* EditorTheme = &USlateThemeManager::Get();

	FString File =  FPaths::EngineContentDir() / TEXT("Slate/Themes/Dark.json");
	//EditorTheme->LoadThemesFromDirectory(File);
	for (int ColorIndex = 0; ColorIndex < int(EStyleColor::User1); ColorIndex++)
	{
		FLinearColor SettingColor = StyleSettings->StyleColors.StyleColors[ColorIndex];
		EStyleColor CurrentColor = EStyleColor(ColorIndex);

		EditorTheme->SetDefaultColor(CurrentColor, SettingColor);
		EditorTheme->ResetActiveColorToDefault(CurrentColor);
	}
	
	FGuid NewGuid = EditorTheme->DuplicateActiveTheme();
	EditorTheme->ApplyTheme(NewGuid);
	EditorTheme->SetCurrentThemeDisplayName(FText::FromString("WallpaperTheme"));
	EditorTheme->RemoveTheme(LastUID);
	LastUID = NewGuid;
	EditorTheme->ApplyDefaultTheme();
	EditorTheme->RemoveTheme(LastUID);
	
	//EditorTheme->LoadThemes();
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
			SAssignNew(WallPaperPanelComboBox, SComboBox< TSharedPtr<FString> >)
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
			int RamdomEditor = FMath::Max(FMath::RandRange(0, Wallpaperlist.Num() - 2), 0);
			int RandomPanel = FMath::Max(FMath::RandRange(0, Wallpaperlist.Num() - 1), 0);
			HandleEditorSelectionChanged(Wallpaperlist[RamdomEditor]);
			HandlePanelSelectionChanged(Wallpaperlist[RandomPanel]);
		}
		else
		{
			int RamdomEditor = FMath::Max(FMath::RandRange(0, Wallpaperlist.Num() - 2), 0);
			int RandomPanel = FMath::Max(FMath::RandRange(0, Wallpaperlist.Num() - 1), 0);
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
		auto lambda = [&]()
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
	if (DirectoryWatcherHandle.IsValid())
	{
		DirectoryWatcherHandle.Reset();
	}

	FDirectoryWatcherModule& DirectoryWatcherModule = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(
		"DirectoryWatcher");
	IDirectoryWatcher* DirectoryWatcher = DirectoryWatcherModule.Get();

	if (DirectoryWatcher && Wallpaperlist.Num() > 0)
	{
		if (WallpaperPlayer->CanPlayvideo())
		{
			const FString Path = (StyleSettings->WallPaperDirectoryPath.Path) + "/steamapps/workshop/content";
			DirectoryWatcher->RegisterDirectoryChangedCallback_Handle(
				Path, IDirectoryWatcher::FDirectoryChanged::CreateRaw(this, &FWallPaperModule::Reimport),
				DirectoryWatcherHandle);
		}
		else
		{
			const FString FilePath = FPaths::ProjectContentDir() / "Wallpaper";
			DirectoryWatcher->RegisterDirectoryChangedCallback_Handle(FilePath,
			                                                          IDirectoryWatcher::FDirectoryChanged::CreateRaw(
				                                                          this, &FWallPaperModule::Reimport),
			                                                          DirectoryWatcherHandle);
		}
	}
}

FText FWallPaperModule::GetEditorComboBoxContent() const
{
	FString OutText = "Editor";
	return FText::FromString(OutText);
}

void FWallPaperModule::Reimport(const TArray<struct FFileChangeData>& FileChanges)
{
	UE_LOG(LogTemp, Log, TEXT("FIle is reimport"))
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
	if (WallpaperPlayer->CanPlayvideo())
	{
		const FString Path = (StyleSettings->WallPaperDirectoryPath.Path) + "/steamapps/workshop/content";

		TArray<FString> Result;
		IFileManager::Get().FindFilesRecursive(Result, *Path,TEXT("*.mp4"), true, true, true);
		MaxNum = Result.Num();
		if (MaxNum > 0)
		{
			for (int32 FileIndex = 0; FileIndex < Result.Num(); ++FileIndex)
			{
				FString FileName = Result[FileIndex];
				WallpaperPath.Add(MakeShareable(new FString(FileName)));
				//FileName.Reverse();
				FString Name = FileName.LeftChop(3);
				int Chop = FileName.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				Name = Name.RightChop(Chop + 1);
				Wallpaperlist.Add(MakeShareable(new FString(Name)));
			}
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("WallpaperTip",
			                                              "Your wallpaper engine path is no vaild.\n please go to editor setting -> wallpaper check you setting"));
			StyleSettings->UseWallpaperEngine = false;
			WallpaperPlayer->SetCanPlayVideo(StyleSettings->UseWallpaperEngine);
			ImportPicTheme();
		}
	}
	else
	{
		StyleSettings->UseWallpaperEngine = false;
		WallpaperPlayer->SetCanPlayVideo(StyleSettings->UseWallpaperEngine);
		ImportPicTheme();
	}

	UToolMenus::Get()->RefreshAllWidgets();
}

void FWallPaperModule::ImportPicTheme()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	const FString FilePath = FPaths::ProjectContentDir() / "Wallpaper";
	
	if (!IFileManager::Get().DirectoryExists(*FilePath))
	{
		IFileManager::Get().MakeDirectory(*FilePath);
		//AssetRegistryModule.Get().AddPath("/Game/Wallpaper");
	}
	
	TArray<FString> FinderFile;
	IFileManager::Get().FindFiles(FinderFile, *FilePath,TEXT("*.uasset"));
	
	//Find plugins path
	FString PluginsPath = FPaths::ProjectPluginsDir()/"Wallpaper";
	if(!IFileManager::Get().DirectoryExists(*PluginsPath))
	{
		PluginsPath = FPaths::EnginePluginsDir()/"Marketplace/Wallpaper";
	}
		
	FString TargetFilePath = PluginsPath/"Content/Cache";
	//clear file
	if(IFileManager::Get().DirectoryExists(*TargetFilePath))
	{
		IFileManager::Get().DeleteDirectory(*(TargetFilePath),false,true);
	}
	IFileManager::Get().MakeDirectory(*TargetFilePath);
	
	MaxNum = FinderFile.Num();
	if (MaxNum > 0)
	{
		for (FString Result : FinderFile)
		{
			Result = Result.LeftChop(7);
			FString LoadFilePath = "/Game/Wallpaper" / Result;
			UTexture* TextureFile = LoadObject<UTexture>(NULL, *LoadFilePath);
			if (TextureFile) //&&asset.PackagePath == FName("/Game/Wallpaper")
				{
				
				FString SourceFilePath = FPaths::ProjectContentDir() / "Wallpaper"/Result+".uasset";
				IFileManager::Get().Copy(*(TargetFilePath/Result+".uasset"), *SourceFilePath);

				
				Wallpaperlist.Add(MakeShareable(new FString(Result)));
				WallpaperPath.Add(MakeShareable(new FString(LoadFilePath)));
			}
		}
		
	}
	else
	{
		LastWallpaperNum = -1;
		Wallpaperlist.Add(MakeShareable(new FString("Default_1")));
		Wallpaperlist.Add(MakeShareable(new FString("Default_2")));
		WallpaperPath.Add(MakeShareable(new FString("/Engine/EngineResources/Black_Low.Black")));
		WallpaperPath.Add(MakeShareable(new FString("/Engine/EngineResources/Black_Low.Black")));
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
	int Num = FMath::Max(FMath::RandRange(0, Wallpaperlist.Num()), 0);
	//找到并且播放
	for (int32 OptionIndex = 0; OptionIndex < Wallpaperlist.Num(); ++OptionIndex)
	{
		if (Wallpaperlist[OptionIndex] == Item)
		{
			Num = OptionIndex;
			break;
		}
	}
	return Num;
}


void FWallPaperModule::HandleEditorSelectionChanged(TSharedPtr<FString> Item)
{
	if (IsValid(WallpaperPlayer))
	{
		if (WallpaperPlayer->CanPlayvideo())
		{
			if (1)
			{
				auto& CoreStyles = FCoreStyle::Get();
				FWindowStyle& WindowsEditor = (FWindowStyle&)CoreStyles.GetWidgetStyle<FWindowStyle>("Window");
				FSlateBrush Editor;
				Editor.SetResourceObject(WallpaperPlayer->GetEditorMaterial());
				Editor.SetImageSize(FVector2D(32, 32));
				WindowsEditor.BackgroundBrush = Editor;
				IsEditorVideoMatarial = true;
			}

			int Num = FindWallpaperIndex(Item);

			WallpaperPlayer->EditorPlay(*WallpaperPath[Num]);
		}
		else
		{
			ApplyEditorBGWithDx12(Item);
		}


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
		if (WallpaperPlayer->CanPlayvideo())
		{
			if (1)
			{
				FSlateBrush panel;
				auto& CoreStyles = FCoreStyle::Get();
				FWindowStyle& WindowsEditor = (FWindowStyle&)CoreStyles.GetWidgetStyle<FWindowStyle>("Window");
				panel.SetResourceObject(WallpaperPlayer->GetPanelMaterial());
				panel.SetImageSize(FVector2D(32, 32));
				WindowsEditor.ChildBackgroundBrush = panel;
				IsPanelVideoMatarial = true;
			}

			int Num = FMath::Max(FMath::RandRange(0, Wallpaperlist.Num()), 0);
			for (int32 OptionIndex = 0; OptionIndex < Wallpaperlist.Num(); ++OptionIndex)
			{
				if (Wallpaperlist[OptionIndex] == Item)
				{
					Num = OptionIndex;
					break;
				}
			}
			WallpaperPlayer->PanelPlay(*WallpaperPath[Num]);
		}
		else
		{
			if (Item.IsValid())
			{
				ApplyPanelBGWithDx12(Item);
			}
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("WallpaperPlayer is no valid"))
}
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FWallPaperModule, WallPaper)

