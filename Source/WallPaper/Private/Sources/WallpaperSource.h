#pragma once

#include "CoreMinimal.h"
#include "WallpaperItem.h"

class FWallpaperSource
{
public:
	static TArray<FWallpaperItem> ScanWallpaperEngine(const FString& SteamRootPath);
	static TArray<FWallpaperItem> ScanProjectTextures(bool& bOutFoundProjectTextures);
};
