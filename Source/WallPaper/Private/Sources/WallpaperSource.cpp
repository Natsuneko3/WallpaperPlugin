#include "Sources/WallpaperSource.h"

#include "Engine/Texture.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"

TArray<FWallpaperItem> FWallpaperSource::ScanWallpaperEngine(const FString& SteamRootPath)
{
	TArray<FWallpaperItem> Items;
	const FString VideoRootPath = SteamRootPath / TEXT("steamapps/workshop/content");

	TArray<FString> VideoFiles;
	IFileManager::Get().FindFilesRecursive(VideoFiles, *VideoRootPath, TEXT("*.mp4"), true, true, true);

	for (const FString& VideoFile : VideoFiles)
	{
		Items.Emplace(FPaths::GetBaseFilename(VideoFile), VideoFile, true);
	}

	return Items;
}

TArray<FWallpaperItem> FWallpaperSource::ScanProjectTextures(bool& bOutFoundProjectTextures)
{
	TArray<FWallpaperItem> Items;
	bOutFoundProjectTextures = false;

	const FString WallpaperContentPath = FPaths::ProjectContentDir() / TEXT("Wallpaper");
	if (!IFileManager::Get().DirectoryExists(*WallpaperContentPath))
	{
		IFileManager::Get().MakeDirectory(*WallpaperContentPath);
	}

	TArray<FString> AssetFiles;
	IFileManager::Get().FindFiles(AssetFiles, *WallpaperContentPath, TEXT("*.uasset"));

	FString PluginPath = FPaths::ProjectPluginsDir() / TEXT("Wallpaper");
	if (!IFileManager::Get().DirectoryExists(*PluginPath))
	{
		PluginPath = FPaths::EnginePluginsDir() / TEXT("Marketplace/Wallpaper");
	}

	const FString CachePath = PluginPath / TEXT("Content/Cache");
	if (IFileManager::Get().DirectoryExists(*CachePath))
	{
		IFileManager::Get().DeleteDirectory(*CachePath, false, true);
	}
	IFileManager::Get().MakeDirectory(*CachePath);

	for (const FString& AssetFile : AssetFiles)
	{
		const FString AssetName = FPaths::GetBaseFilename(AssetFile);
		const FString LoadPath = FString(TEXT("/Game/Wallpaper")) / AssetName;
		UTexture* Texture = LoadObject<UTexture>(nullptr, *LoadPath);
		if (!Texture)
		{
			continue;
		}

		const FString SourceFilePath = WallpaperContentPath / (AssetName + TEXT(".uasset"));
		IFileManager::Get().Copy(*(CachePath / (AssetName + TEXT(".uasset"))), *SourceFilePath);

		Items.Emplace(AssetName, LoadPath, false);
	}

	bOutFoundProjectTextures = Items.Num() > 0;
	if (!bOutFoundProjectTextures)
	{
		Items.Emplace(TEXT("Default_1"), TEXT("/Engine/EngineResources/Black_Low.Black"), false);
		Items.Emplace(TEXT("Default_2"), TEXT("/Engine/EngineResources/Black_Low.Black"), false);
	}

	return Items;
}
