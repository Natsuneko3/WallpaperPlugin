// Copyright 2023 Natsu Neko, Inc. All Rights Reserved.


#include "WallPaperSetting.h"
#include "Settings/EditorStyleSettings.h"

UWallPaperSetting::UWallPaperSetting()
{
	
	InitCoreStyle();
	ResetStyleColor();
	EditorUseGrid = GetDefault<UEditorStyleSettings>()->bUseGrid;
}


void UWallPaperSetting::InitCoreStyle()
{
	/*Windows_Backgroud = FCoreStyle::Get().GetWidgetStyle<FWindowStyle>("Window").BackgroundBrush;
	Windows_Backgroud.TintColor = FColor(1, 1, 1, 1);*/

	/*Windows_Chlid_Backgroud = FCoreStyle::Get().GetWidgetStyle<FWindowStyle>("Window").ChildBackgroundBrush;
	Windows_Chlid_Backgroud.TintColor = FColor(1, 1, 1, 1);*/
	FSlateBrush MenuBrush;
	UTexture* MenuBrushBackGround = LoadObject<UTexture>(NULL,TEXT("/WallPaper/WallPaperEngine/Backgound/wallhaven-966dxk"));
	MenuBrush.SetResourceObject(MenuBrushBackGround);
	Menu_Background = MenuBrush;
	Menu_Background.TintColor = FLinearColor(0.2, 0.2, 0.2, 1);
}


void UWallPaperSetting::ResetStyleColor()
{
	StyleColors.StyleColors[(int)EStyleColor::Black]= COLOR("#000000FF");
	StyleColors.StyleColors[(int)EStyleColor::Title]= COLOR("#15151566");
	StyleColors.StyleColors[(int)EStyleColor::Background]= COLOR("#15151533");
	StyleColors.StyleColors[(int)EStyleColor::WindowBorder]= COLOR("0F0F0FFF");
	StyleColors.StyleColors[(int)EStyleColor::Foldout]= COLOR("0F0F0FFF");
	StyleColors.StyleColors[(int)EStyleColor::Input]= COLOR("0F0F0FCC");
	StyleColors.StyleColors[(int)EStyleColor::InputOutline]= COLOR("383838FF");
	StyleColors.StyleColors[(int)EStyleColor::Recessed]= COLOR("#1A1A1A33");
	StyleColors.StyleColors[(int)EStyleColor::Panel]= COLOR("#2424244D");
	StyleColors.StyleColors[(int)EStyleColor::Header]= COLOR("#2F2F2F99");
	StyleColors.StyleColors[(int)EStyleColor::Dropdown]= COLOR("#38383866");
	StyleColors.StyleColors[(int)EStyleColor::DropdownOutline]= COLOR("#4C4C4CFF");
	StyleColors.StyleColors[(int)EStyleColor::Hover]= COLOR("#575757FF");
	StyleColors.StyleColors[(int)EStyleColor::Hover2]= COLOR("#808080FF");

	StyleColors.StyleColors[(int)EStyleColor::White]= COLOR("#FFFFFFB3");
	StyleColors.StyleColors[(int)EStyleColor::White25]= COLOR("#FFFFFF40");
	StyleColors.StyleColors[(int)EStyleColor::Highlight]= COLOR("#0070E0FF");

	StyleColors.StyleColors[(int)EStyleColor::Primary]= COLOR("#0070E0FF");
	StyleColors.StyleColors[(int)EStyleColor::PrimaryHover]= COLOR("#0E86FFFF");
	StyleColors.StyleColors[(int)EStyleColor::PrimaryPress]= COLOR("#0050A0FF");
	StyleColors.StyleColors[(int)EStyleColor::Secondary]= COLOR("#383838FF");

	StyleColors.StyleColors[(int)EStyleColor::Foreground]= COLOR("#C0C0C0FF");
	StyleColors.StyleColors[(int)EStyleColor::ForegroundHover]= COLOR("#FFFFFFFF");
	StyleColors.StyleColors[(int)EStyleColor::ForegroundInverted]= COLOR("0F0F0FFF");
	StyleColors.StyleColors[(int)EStyleColor::ForegroundHeader]= COLOR("#C8C8C8FF");

	StyleColors.StyleColors[(int)EStyleColor::Select]= COLOR("#0070E0FF");
	StyleColors.StyleColors[(int)EStyleColor::SelectInactive]= COLOR("#40576F");
	StyleColors.StyleColors[(int)EStyleColor::SelectParent]= COLOR("#2C323AFF");
	StyleColors.StyleColors[(int)EStyleColor::SelectHover]= COLOR("#242424FF");
	StyleColors.StyleColors[(int)EStyleColor::Notifications]= COLOR("464B50FF");

	// if select ==  primary shouldnt we have a select pressed which is the same as primary press?

	StyleColors.StyleColors[(int)EStyleColor::AccentBlue]= COLOR("#26BBFFFF");
	StyleColors.StyleColors[(int)EStyleColor::AccentPurple]= COLOR("#A139BFFF");
	StyleColors.StyleColors[(int)EStyleColor::AccentPink]= COLOR("#FF729CFF");
	StyleColors.StyleColors[(int)EStyleColor::AccentRed]= COLOR("#FF4040FF");
	StyleColors.StyleColors[(int)EStyleColor::AccentOrange]= COLOR("#FE9B07FF");
	StyleColors.StyleColors[(int)EStyleColor::AccentYellow]= COLOR("#FFDC1AFF");
	StyleColors.StyleColors[(int)EStyleColor::AccentGreen]= COLOR("#8BC24AFF");
	StyleColors.StyleColors[(int)EStyleColor::AccentBrown]= COLOR("#804D39FF");
	StyleColors.StyleColors[(int)EStyleColor::AccentBlack]= COLOR("#242424FF");
	StyleColors.StyleColors[(int)EStyleColor::AccentGray]= COLOR("#808080FF");
	StyleColors.StyleColors[(int)EStyleColor::AccentWhite]= COLOR("#FFFFFFFF");
	StyleColors.StyleColors[(int)EStyleColor::AccentFolder]= COLOR("#B68F55FF");

	StyleColors.StyleColors[(int)EStyleColor::Warning]= COLOR("#FFB800FF");
	StyleColors.StyleColors[(int)EStyleColor::Error]= COLOR("#EF3535FF");
	StyleColors.StyleColors[(int)EStyleColor::Success]= COLOR("#1FE44BFF");
}
