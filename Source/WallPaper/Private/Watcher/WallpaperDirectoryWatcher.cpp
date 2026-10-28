#include "Watcher/WallpaperDirectoryWatcher.h"

#include "DirectoryWatcherModule.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"

void FWallpaperDirectoryWatcher::Watch(FDelegateHandle& InOutHandle, FString& InOutDirectory, const FString& Directory, const IDirectoryWatcher::FDirectoryChanged& Callback)
{
	if (Directory.IsEmpty())
	{
		Stop(InOutHandle, InOutDirectory);
		return;
	}

	FString NormalizedDirectory = FPaths::ConvertRelativePathToFull(Directory);
	FPaths::NormalizeDirectoryName(NormalizedDirectory);
	if (!IFileManager::Get().DirectoryExists(*NormalizedDirectory))
	{
		Stop(InOutHandle, InOutDirectory);
		return;
	}
	if (InOutHandle.IsValid() && FPaths::IsSamePath(InOutDirectory, NormalizedDirectory))
	{
		return;
	}

	Stop(InOutHandle, InOutDirectory);

	FDirectoryWatcherModule& DirectoryWatcherModule = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(TEXT("DirectoryWatcher"));
	IDirectoryWatcher* DirectoryWatcher = DirectoryWatcherModule.Get();
	if (!DirectoryWatcher)
	{
		return;
	}

	if (DirectoryWatcher->RegisterDirectoryChangedCallback_Handle(NormalizedDirectory, Callback, InOutHandle, IDirectoryWatcher::IncludeDirectoryChanges))
	{
		InOutDirectory = MoveTemp(NormalizedDirectory);
	}
	else
	{
		InOutHandle.Reset();
	}
}

void FWallpaperDirectoryWatcher::Stop(FDelegateHandle& InOutHandle, FString& InOutDirectory)
{
	if (!InOutHandle.IsValid())
	{
		InOutDirectory.Reset();
		return;
	}

	if (FModuleManager::Get().IsModuleLoaded(TEXT("DirectoryWatcher")))
	{
		FDirectoryWatcherModule& DirectoryWatcherModule = FModuleManager::GetModuleChecked<FDirectoryWatcherModule>(TEXT("DirectoryWatcher"));
		if (IDirectoryWatcher* DirectoryWatcher = DirectoryWatcherModule.Get())
		{
			DirectoryWatcher->UnregisterDirectoryChangedCallback_Handle(InOutDirectory, InOutHandle);
		}
	}

	InOutHandle.Reset();
	InOutDirectory.Reset();
}
