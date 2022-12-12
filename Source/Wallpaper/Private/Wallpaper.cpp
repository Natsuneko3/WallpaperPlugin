// Copyright Natsu Neko, Inc. All Rights Reserved.

#include "Wallpaper.h"

#include "DetailLayoutBuilder.h"
#include "DirectoryWatcherModule.h"

#include "ISettingsSection.h"
#include "WallPaperSetting.h"
#include "WallpaperCommands.h"
#include "WallPaperEditor.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Classes/EditorStyleSettings.h"
#include "EditorUtilityWidget.h"
#include "IBlutilityModule.h"
#include "IContentBrowserDataModule.h"
#include "IDirectoryWatcher.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructure.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"

#include "LevelEditor.h"
#include "RenderGraphResources.h"


#include "Styling/SlateStyle.h"
#include "Engine/Blueprint.h"
#include "Misc/MessageDialog.h"

#define LOCTEXT_NAMESPACE "FWallpaperModule"

void FWallPaperModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FString AssetPath = "/WallPaper/WallPaperEngine/UMG";
	UPackage* EditorTexturePack = CreatePackage(*AssetPath);
	WallpaperPlayer = NewObject<UWallPaperBrush>(EditorTexturePack, FName("WallpaperPlayer"), RF_MarkAsRootSet);
	WallpaperPlayer->SetCanPlayVideo(StyleSettings->UseWallpaperEngine);
	bLasyType = StyleSettings->UseWallpaperEngine;

	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FWallPaperModule::CheckTimer);

	ImportWallpaper();
	InitialEditorStyle();
	ApplyEditorStyle();
	CreateWatcher();


	GetMutableDefault<UEditorStyleSettings>()->bUseGrid = StyleSettings->EditorUseGrid;
	ApplyMenuBackGround();

	SetSetting();
	InitialTheme();
	//GEditor->GetTimerManager()->SetTimerForNextTick([this](){CheckTimer();});

	FWallpaperStyle::Initialize();
	FWallpaperStyle::ReloadTextures();
	FWallpaperCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FWallpaperCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FWallPaperModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FWallPaperModule::RegisterMenus));
}

void FWallPaperModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	//FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").UnregisterSettings("Editor", "General", "WallPaper");
	//UToolMenus::UnRegisterStartupCallback(this);
	//UE_LOG(LogTemp,Log,TEXT("shutdown moud"));
	

	//ClearCahce
	/*FString PluginsPath = FPaths::ProjectPluginsDir()/"Wallpaper";
	if(!IFileManager::Get().DirectoryExists(*PluginsPath))
	{
		PluginsPath = FPaths::EnginePluginsDir()/"Wallpaper";
	}
		
	FString TargetFilePath = PluginsPath/"Content/Cache";
	if(IFileManager::Get().DirectoryExists(*TargetFilePath))
	{
		IFileManager::Get().DeleteDirectory(*(TargetFilePath),false,true);
	}*/
	
}

bool FWallPaperModule::OnSettingModified()
{
	WallpaperPlayer->SetCanPlayVideo(StyleSettings->UseWallpaperEngine);
	if (bLasyType != StyleSettings->UseWallpaperEngine || LastWallpaperPath != StyleSettings->WallPaperDirectoryPath.Path)
	{
		bLasyType = StyleSettings->UseWallpaperEngine;
		TArray<struct FFileChangeData> FileChanges;
		Reimport(FileChanges);
		
	}
	CreateWatcher();
	GetMutableDefault<UEditorStyleSettings>()->bUseGrid = StyleSettings->EditorUseGrid;
	ApplyMenuBackGround();
	CheckTimer();
	auto& CoreStyles = FCoreStyle::Get();
	FWindowStyle& WindowsEditor = (FWindowStyle&)CoreStyles.GetWidgetStyle<FWindowStyle>("Window");
	auto& EditorStyles = FEditorStyle::Get();
	FSlateBrush& SolidBackground = *(FSlateBrush*)EditorStyles.GetBrush("Graph.Panel.SolidBackground");


	//WindowsEditor.ChildBackgroundBrush = StyleSettings->Menu_Background;

	WindowsEditor.BackgroundBrush.TintColor = FLinearColor(FVector(StyleSettings->EditorOpacity));
	SolidBackground.TintColor = FLinearColor(FVector(StyleSettings->PanelOpacity));


	return true;
}


void FWallPaperModule::InitialEditorStyle()
{
	auto& EditorStyles = FEditorStyle::Get();
	FSlateBrush& SolidBackground = *(FSlateBrush*)EditorStyles.GetBrush("Graph.Panel.SolidBackground");

	auto& CoreStyles = FCoreStyle::Get();
	FWindowStyle& WindowsEditor = (FWindowStyle&)CoreStyles.GetWidgetStyle<FWindowStyle>("Window");

	if (!IsValid(StyleSettings->Menu_Background.GetResourceObject()))
	{
		FSlateBrush ChildBackGround;
		UTexture* ChildBackTexture = LoadObject<UTexture>(NULL,TEXT("/Wallpaper/wallhaven-4g62qe"));
		ChildBackGround.SetResourceObject(ChildBackTexture);
		ChildBackGround.SetImageSize(FVector2D(32, 32));

		StyleSettings->Menu_Background = ChildBackGround;
		StyleSettings->Menu_Background.TintColor = FLinearColor(0.3, 0.3, 0.3, 0.3);
	}
	WindowsEditor.ChildBackgroundBrush = StyleSettings->Menu_Background;
	WindowsEditor.ChildBackgroundBrush.TintColor = StyleSettings->Menu_Background.TintColor;

	if (IsValid(WallpaperPlayer))
	{
		//设置主界面与材质界面图，通过material
		FSlateBrush Editor;
		FSlateBrush panel;
		if (WallpaperPlayer->CanPlayvideo())
		{
			if (Wallpaperlist.Num() == 0)
			{
				UTexture* EditorTexture = LoadObject<UTexture>(NULL,TEXT("/Wallpaper/wallhaven-966dxk"));
				UTexture* PanelTexture = LoadObject<UTexture>(NULL,TEXT("/Wallpaper/wallhaven-4g62qe"));
				Editor.SetResourceObject(EditorTexture);
				Editor.SetImageSize(FVector2D(32, 32));

				panel.SetResourceObject(PanelTexture);
				panel.SetImageSize(FVector2D(32, 32));

				WindowsEditor.BackgroundBrush = Editor;
				WindowsEditor.BackgroundBrush.TintColor = FLinearColor(FVector(StyleSettings->EditorOpacity));

				SolidBackground = panel;
				SolidBackground.TintColor = FLinearColor(FVector(StyleSettings->PanelOpacity));
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


	//设置菜单背景
}

void FWallPaperModule::InitialTheme()
{
	StyleSettings->Docking_MajorTab.ContentAreaBrush.TintColor = FLinearColor(1, 1, 1, 0.4);
	StyleSettings->Docking_Tab.NormalBrush.TintColor = FLinearColor(0.02, 0.02, 0.02, 0.4);
	StyleSettings->Docking_Tab.ContentAreaBrush.TintColor = FLinearColor(0.04, 0.04, 0.04, 0.3);
	StyleSettings->Docking_Tab.ForegroundBrush.TintColor = FLinearColor(0.048172, 0.048172, 0.048172, 0.5);
	StyleSettings->ContentBrowser_TopBar_GroupBorder.TintColor = FLinearColor(0.3, 0.3, 0.3, 0.2);
	StyleSettings->CategoryMiddle_Hovered.TintColor = FLinearColor(0.5, 0.5, 0.5, 0.6);
	StyleSettings->ToolPanel_GroupBorder.TintColor = FLinearColor(0.6, 0.6, 0.6, 0.6);
	StyleSettings->ToolBar_Background.TintColor = FLinearColor(0.2, 0.2, 0.2, 0.3);
	StyleSettings->ToolPanel_DarkGroupBorder.TintColor = FLinearColor(0.5, 0.5, 0.5, 0.4);
	StyleSettings->PlacementBrowser_Tab.UncheckedImage.TintColor = FLinearColor(0.5, 0.5, 0.5, 0.2);
	FSlateStyleSet* EditorStyles = (FSlateStyleSet*)&FEditorStyle::Get();
	EditorStyles->Set("ContentBrowser.TopBar.GroupBorder", &StyleSettings->ContentBrowser_TopBar_GroupBorder);
	EditorStyles->Set("DetailsView.CategoryMiddle", &StyleSettings->CategoryMiddle_Hovered);
	EditorStyles->Set("ToolPanel.GroupBorder", &StyleSettings->ToolPanel_GroupBorder);
	EditorStyles->Set("ToolPanel.DarkGroupBorder", &StyleSettings->ToolPanel_DarkGroupBorder);


	auto& CoreStyles = FCoreStyle::Get();
	FWindowStyle& WindowsEditor = (FWindowStyle&)CoreStyles.GetWidgetStyle<FWindowStyle>("Window");
	(FDockTabStyle&)CoreStyles.GetWidgetStyle<FDockTabStyle>("Docking.MajorTab") = StyleSettings->Docking_MajorTab;
	(FDockTabStyle&)CoreStyles.GetWidgetStyle<FDockTabStyle>("Docking.Tab") = StyleSettings->Docking_Tab;
	*(FSlateBrush*)CoreStyles.GetBrush("ToolBar.Background") = StyleSettings->ToolBar_Background;

	(FCheckBoxStyle&)FEditorStyle::GetWidgetStyle<FCheckBoxStyle>("PlacementBrowser.Tab") = StyleSettings->PlacementBrowser_Tab;
}

void FWallPaperModule::PluginButtonClicked()
{
	int RamdomEditor = FMath::Max(FMath::RandRange(0, Wallpaperlist.Num() - 2), 0);
	int RandomPanel = FMath::Max(FMath::RandRange(0, Wallpaperlist.Num() - 1), 0);
	ApplyEditorBGWithDx12(Wallpaperlist[RamdomEditor]);
	ApplyPanelBGWithDx12(Wallpaperlist[RandomPanel]);
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
	IsEditorVideoMatarial = false;
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
	IsEditorVideoMatarial = false;
	FSlateBrush panel;
	int index = FindWallpaperIndex(Item);
	FString FilePath = *WallpaperPath[index];
	UTexture* Texture = LoadObject<UTexture>(NULL, *FilePath);
	if (Texture)
	{
		panel.SetResourceObject(Texture);
		panel.SetImageSize(FVector2D(32, 32));
	}

	auto& EditorStyles = FEditorStyle::Get();
	FSlateBrush& SolidBackground = *(FSlateBrush*)EditorStyles.GetBrush("Graph.Panel.SolidBackground");
	SolidBackground = panel;
	SolidBackground.TintColor = FLinearColor(FVector(StyleSettings->PanelOpacity));
}


void FWallPaperModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);


	TSharedRef<SWidget> WallpaperWidget = SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		  .AutoWidth()
		  .VAlign(VAlign_Center)
		[
			SNew(SCheckBox)
			.ToolTipText(LOCTEXT("CheckBoxTooltip", "Enable Wallpaper engine"))
			.IsChecked_Lambda(
				               [=]() -> ECheckBoxState
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
		  .VAlign(VAlign_Center)
		[
			SAssignNew(WallPaperPanelComboBox, SComboBox< TSharedPtr<FString> >)
				.OptionsSource(&Wallpaperlist)
				.ForegroundColor(FLinearColor::Black)
			//.InitiallySelectedItem(Wallpaperlist[MaxNum])
				.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
			                                                                    {
				                                                                    return SNew(STextBlock)

					                                                                    .Text(FText::FromString(*Item.Get()));
			                                                                    })
				.OnSelectionChanged_Lambda([this](TSharedPtr<FString> Item, ESelectInfo::Type SelectionType)
			                                                                    {
				                                                                    HandleEditorSelectionChanged(Item);
			                                                                    })
				//.ComboBoxStyle(FMacGraphicsSwitchingStyle::Get(), "MacGraphicsSwitcher.ComboBox")
				.Content()
			[
				SNew(STextBlock)
				.Text_Raw(this, &FWallPaperModule::GetEditorComboBoxContent)
			]

		]
		+ SHorizontalBox::Slot()
		  .AutoWidth()
		  .VAlign(VAlign_Center)
		[
			SAssignNew(WallPaperPanelComboBox, SComboBox< TSharedPtr<FString> >)
				.OptionsSource(&Wallpaperlist)
				.ForegroundColor(FLinearColor::Black)
				.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
			                                                                    {
				                                                                    return SNew(STextBlock)

					                                                                    .Text(FText::FromString(*Item.Get()));
			                                                                    })
				.OnSelectionChanged_Lambda([this](TSharedPtr<FString> Item, ESelectInfo::Type SelectionType)
			                                                                    {
				                                                                    HandlePanelSelectionChanged(Item);
			                                                                    })
				//.ComboBoxStyle(FEditorStyle::Get(), "TutorialEditableText.Toolbar.ComboBox")
				.Content()
			[
				SNew(STextBlock)
				.Text_Raw(this, &FWallPaperModule::GetPanelComboBoxContent)

			]

		];


	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("OpenBlueprint");
			{
				//ToolbarMenu->re
				FToolMenuEntry& Entry = Section.
					AddEntry(FToolMenuEntry::InitWidget("Wallpaper", WallpaperWidget, LOCTEXT("Wallpaper", "WallpaperEditor")));
				Entry.SetCommandList(PluginCommands);
			}
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
	SettingS->OnResetDefaults().BindLambda([=]()-> bool
	{
		StyleSettings->InitCoreStyle();
		InitialEditorStyle();

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
		FConfigCacheIni::LoadGlobalIniFile(ConfigNeedToReset[0], *FPaths::GetBaseFilename(ConfigNeedToReset[0]), nullptr, true);
		GetMutableDefault<UWallPaperSetting>()->ReloadConfig(nullptr, nullptr,
		                                                     UE4::LCPF_PropagateToInstances | UE4::LCPF_PropagateToChildDefaultObjects);
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
			if (Wallpaperlist.Num() > 0)
			{
				int RamdomEditor = FMath::Max(FMath::RandRange(0, Wallpaperlist.Num() - 2), 0);
				int RandomPanel = FMath::Max(FMath::RandRange(0, Wallpaperlist.Num() - 1), 0);
				HandleEditorSelectionChanged(Wallpaperlist[RamdomEditor]);
				HandlePanelSelectionChanged(Wallpaperlist[RandomPanel]);
			}
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

	FDirectoryWatcherModule& DirectoryWatcherModule = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>("DirectoryWatcher");
	IDirectoryWatcher* DirectoryWatcher = DirectoryWatcherModule.Get();

	if (DirectoryWatcher)
	{
		const FString Path = (StyleSettings->WallPaperDirectoryPath.Path) + "/steamapps/workshop/content";
		const FString FilePath = FPaths::ProjectContentDir() / "Wallpaper";//FPaths::ProjectContentDir() / "Wallpaper";
		if (WallpaperPlayer->CanPlayvideo())
		{
			if (IFileManager::Get().DirectoryExists(*Path))
			{
				DirectoryWatcher->RegisterDirectoryChangedCallback_Handle(
					Path, IDirectoryWatcher::FDirectoryChanged::CreateRaw(this, &FWallPaperModule::Reimport), DirectoryWatcherHandle);
			}
		}
		else
		{
			if (IFileManager::Get().DirectoryExists(*FilePath))
			{
				DirectoryWatcher->RegisterDirectoryChangedCallback_Handle(FilePath,
				                                                          IDirectoryWatcher::FDirectoryChanged::CreateRaw(
					                                                          this, &FWallPaperModule::Reimport), DirectoryWatcherHandle);
			}
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
	if (StyleSettings->UseWallpaperEngine)
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
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("WallpaperTip", "Your wallpaper engine path is no vaild.\n please go to editor setting -> wallpaper check you setting"));
			StyleSettings->UseWallpaperEngine = false;
			WallpaperPlayer->SetCanPlayVideo(StyleSettings->UseWallpaperEngine);
			ImportPicTheme();
		}
	}
	else
	{
		//把图片list写入wallpaperlist
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
		PluginsPath = FPaths::EnginePluginsDir()/"Wallpaper";
	}
		
	FString TargetFilePath = PluginsPath/"Content/Cache";
	//clear file
	if(IFileManager::Get().DirectoryExists(*TargetFilePath))
	{
		IFileManager::Get().DeleteDirectory(*(TargetFilePath),false,true);
	}
	IFileManager::Get().MakeDirectory(*TargetFilePath);
	
	MaxNum = FinderFile.Num();
	if(MaxNum > 0)
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
				WallpaperPath.Add(MakeShareable(new FString("/Wallpaper/Cache"/Result)));
				//UE_LOG(LogTemp,Warning,TEXT("DataNUm:%s"),*asset.PackagePath.ToString());
			}
		}
		IFileManager::Get().DeleteDirectory(*TargetFilePath);
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
	int Num = FMath::RandRange(0, Wallpaperlist.Num());
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
		if (WallpaperPlayer->CanPlayvideo() && Wallpaperlist.Num() > 0)
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
	}
}

void FWallPaperModule::HandlePanelSelectionChanged(TSharedPtr<FString> Item)
{
	if (IsValid(WallpaperPlayer))
	{
		if (WallpaperPlayer->CanPlayvideo() && Wallpaperlist.Num() > 0)
		{
			if (1)
			{
				FSlateBrush panel;
				auto& EditorStyles = FEditorStyle::Get();
				panel.SetResourceObject(WallpaperPlayer->GetPanelMaterial());
				panel.SetImageSize(FVector2D(32, 32));
				FSlateBrush& SolidBackground = *(FSlateBrush*)EditorStyles.GetBrush("Graph.Panel.SolidBackground");
				SolidBackground = panel;

				IsPanelVideoMatarial = true;
			}

			int Num = FMath::RandRange(0, Wallpaperlist.Num());
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

void FWallPaperModule::ApplyEditorStyle()
{
	FSlateStyleSet* EditorStyles = (FSlateStyleSet*)&FEditorStyle::Get();
	EditorStyles->Set("ContentBrowser.TopBar.GroupBorder", &StyleSettings->ContentBrowser_TopBar_GroupBorder);
	EditorStyles->Set("DetailsView.CategoryMiddle", &StyleSettings->CategoryMiddle_Hovered);
	EditorStyles->Set("ToolPanel.GroupBorder", &StyleSettings->ToolPanel_GroupBorder);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FWallPaperModule, Wallpaper)
