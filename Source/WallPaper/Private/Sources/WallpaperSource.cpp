#include "Sources/WallpaperSource.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/ARFilter.h"
#include "Engine/Texture.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"

TArray<FWallpaperItem> FWallpaperSource::ScanWallpaperEngine(const FString& SteamRootPath)
{
	const FString VideoRootPath = FPaths::ConvertRelativePathToFull(SteamRootPath / TEXT("steamapps/workshop/content"));
	TArray<FString> VideoFiles;
	IFileManager::Get().FindFilesRecursive(VideoFiles, *VideoRootPath, TEXT("*.mp4"), true, false, true);
	VideoFiles.Sort();

	TArray<FWallpaperItem> Items;
	Items.Reserve(VideoFiles.Num());
	for (const FString& VideoFile : VideoFiles)
	{
		Items.Emplace(FPaths::GetBaseFilename(VideoFile), VideoFile, true);
	}

	return Items;
}

TArray<FWallpaperItem> FWallpaperSource::ScanProjectTextures()
{
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
	FARFilter Filter;
	Filter.PackagePaths.Add(TEXT("/Game/Wallpaper"));
	Filter.ClassPaths.Add(UTexture::StaticClass()->GetClassPathName());
	Filter.bRecursivePaths = true;
	Filter.bRecursiveClasses = true;

	// The editor discovers assets asynchronously; registry events refresh this list when discovery completes.
	TArray<FAssetData> Assets;
	AssetRegistry.GetAssets(Filter, Assets);
	TArray<FWallpaperItem> Items;
	Items.Reserve(Assets.Num());
	for (const FAssetData& Asset : Assets)
	{
		Items.Emplace(Asset.AssetName.ToString(), Asset.GetSoftObjectPath().ToString(), false);
	}
	Items.Sort(
		[](const FWallpaperItem& Left, const FWallpaperItem& Right)
		{
			return Left.ResourcePath < Right.ResourcePath;
		});

	if (Items.IsEmpty())
	{
		Items.Emplace(TEXT("Default_1"), TEXT("/WallPaper/WallPaperEngine/Backgound/Default_1.Default_1"), false);
		Items.Emplace(TEXT("Default_2"), TEXT("/WallPaper/WallPaperEngine/Backgound/Default_2.Default_2"), false);
	}

	return Items;
}
