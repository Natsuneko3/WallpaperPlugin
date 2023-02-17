// Copyright 2023 Natsu Neko, Inc. All Rights Reserved.

#include "WallPaperSetting.h"
#include "Classes/EditorStyleSettings.h"

UWallPaperSetting::UWallPaperSetting()
{
	
	InitCoreStyle();
	InitEditorStyle();
	EditorUseGrid = GetDefault<UEditorStyleSettings>()->bUseGrid;
}


void UWallPaperSetting::InitCoreStyle()
{
	/*Windows_Backgroud = FCoreStyle::Get().GetWidgetStyle<FWindowStyle>("Window").BackgroundBrush;
	Windows_Backgroud.TintColor = FColor(1, 1, 1, 1);*/

	/*Windows_Chlid_Backgroud = FCoreStyle::Get().GetWidgetStyle<FWindowStyle>("Window").ChildBackgroundBrush;
	Windows_Chlid_Backgroud.TintColor = FColor(1, 1, 1, 1);*/
	FSlateBrush MenuBrush;
	UTexture* MenuBrushBackGround = LoadObject<UTexture>(NULL,TEXT("/Engine/EngineResources/Black_Low.Black"));
	MenuBrush.SetResourceObject(MenuBrushBackGround);
	Menu_Background = MenuBrush;
	Menu_Background.TintColor = FLinearColor(0.2, 0.2, 0.2, 1);
}

void UWallPaperSetting::InitEditorStyle()
{
	Docking_Tab = FCoreStyle::Get().GetWidgetStyle<FDockTabStyle>("Docking.Tab");
	Docking_MajorTab = FCoreStyle::Get().GetWidgetStyle<FDockTabStyle>("Docking.MajorTab");
	ContentBrowser_TopBar_GroupBorder = *FEditorStyle::GetBrush("ContentBrowser.TopBar.GroupBorder");
	CategoryMiddle_Hovered = *FEditorStyle::GetBrush("DetailsView.CategoryMiddle_Hovered");
	ToolPanel_GroupBorder = *FEditorStyle::GetBrush("ToolPanel.GroupBorder");
	ToolBar_Background = *FCoreStyle::Get().GetBrush("ToolBar.Background");
	ToolPanel_DarkGroupBorder = *FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder");
	PlacementBrowser_Tab = FEditorStyle::GetWidgetStyle<FCheckBoxStyle>("PlacementBrowser.Tab");
	
}

