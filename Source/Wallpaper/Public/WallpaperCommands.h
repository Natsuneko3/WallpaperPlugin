// Copyright 2023 Natsu Neko, Inc. All Rights Reserved.
#pragma once


#include "CoreMinimal.h"
#include "WallPaper.h"
#include "Framework/Commands/Commands.h"
#include "WallpaperStyle.h"

class FWallpaperCommands : public TCommands<FWallpaperCommands>
{
public:
	FWallpaperCommands()
		: TCommands<FWallpaperCommands>(TEXT("Wallpaper"), NSLOCTEXT("Contexts", "Wallpaper", "Wallpaper Plugin"), NAME_None,
										FWallpaperStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> PluginAction;
};
