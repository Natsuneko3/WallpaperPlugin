// Copyright 2023 Natsu Neko, Inc. All Rights Reserved.

#include "WallpaperCommands.h"
#define LOCTEXT_NAMESPACE "FWallpaperModule"

void FWallpaperCommands::RegisterCommands()
{
	UI_COMMAND(Excution, "ChangeWallpaper", "Random Wallpaper", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Shift|EModifierKey::Control,EKeys::C));
}

#undef LOCTEXT_NAMESPACE
