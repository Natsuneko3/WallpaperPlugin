// Copyright Natsu Neko, Inc. All Rights Reserved.

#include "WallpaperCommands.h"
#define LOCTEXT_NAMESPACE "FWallpaperModule"

void FWallpaperCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Wallpaper", "Execute Wallpaper action",EUserInterfaceActionType::Button,FInputChord(EModifierKey::Shift|EModifierKey::Control,EKeys::C));
	
}

#undef LOCTEXT_NAMESPACE
