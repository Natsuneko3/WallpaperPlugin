#include  "WallpaperCustomEditor.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "WallPaper.h"
#include "Widgets/Input/SButton.h"


#define LOCTEXT_NAMESPACE "FWallPaperModule"

void UWallpaperCustomEditor::CustomizeDetails(IDetailLayoutBuilder& DetailLyoutBuilder)
{
	IDetailCategoryBuilder& WallPaperCustomTImerCategory = DetailLyoutBuilder.EditCategory(TEXT("WallPaper"));
	WallPaperCustomTImerCategory.AddCustomRow(FText::FromString("ChangeThemeTime"))
						  .WholeRowWidget
	[
	SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
			.Text(LOCTEXT("ImportTheme", "Import Theme"))
			.HAlign(HAlign_Center)
			.ToolTipText(LOCTEXT("ImportThemeButtonTip", "Import Theme From a file.\nBoth *.UTheme and *.Ini are Supported."))
			
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
			.Text(LOCTEXT("PackageTheme", "Package Current Theme"))
			.ToolTipText(LOCTEXT("PackageThemeButtonTip", "Package Current Theme as a *.UTheme file.\n"
			                     "A UTheme file include all Contents referenced by the ConfigFile in UEEditorCustomize content floder. You can share it to other people.\n"
			                     "You can also export only a ini file. But it will not contain any Content."))
			.HAlign(HAlign_Center)
			
			]
		]
		+ SVerticalBox::Slot()
		[
			SNew(SButton)
			.Text(LOCTEXT("ImportBuiltInTheme", "Import Built-In Theme"))
			.HAlign(HAlign_Center)
			
		]
		+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
			.Text(LOCTEXT("ResetEditorStyle", "Reset Editor Style"))
			.HAlign(HAlign_Center)
			.ToolTipText(LOCTEXT("ResetEditorStyleButtonTip", "Reset all settings that in \"Editor Style\" Category"))
			
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
			.Text(LOCTEXT("ResetCoreStyle", "Reset Core Style"))
			.HAlign(HAlign_Center)
			.ToolTipText(LOCTEXT("ResetCoreStyleButtonTip", "Reset all settings that in \"Core Style\" Category"))
			
			]

		]
		];
}

#undef LOCTEXT_NAMESPACE