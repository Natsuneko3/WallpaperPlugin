#pragma once

#include "CoreMinimal.h"

struct FWallpaperItem
{
	FString DisplayName;
	FString ResourcePath;
	bool bIsVideo = false;

	FWallpaperItem() = default;

	FWallpaperItem(FString InDisplayName, FString InResourcePath, bool bInIsVideo)
		: DisplayName(MoveTemp(InDisplayName))
		  , ResourcePath(MoveTemp(InResourcePath))
		  , bIsVideo(bInIsVideo)
	{
	}
};
