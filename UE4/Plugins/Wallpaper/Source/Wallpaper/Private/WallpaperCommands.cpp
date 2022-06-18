#include "WallpaperCommands.h"
#define LOCTEXT_NAMESPACE "FWallpaperModule"

void FWallpaperCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Wallpaper", "Execute Wallpaper action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
