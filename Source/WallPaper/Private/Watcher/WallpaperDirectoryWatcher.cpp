#include "Watcher/WallpaperDirectoryWatcher.h"

#include "DirectoryWatcherModule.h"
#include "HAL/FileManager.h"
#include "Modules/ModuleManager.h"

void FWallpaperDirectoryWatcher::Watch(
	FDelegateHandle& InOutHandle,
	FString& InOutDirectory,
	const FString& Directory,
	const IDirectoryWatcher::FDirectoryChanged& Callback)
{
	Stop(InOutHandle, InOutDirectory);

	if (Directory.IsEmpty() || !IFileManager::Get().DirectoryExists(*Directory))
	{
		return;
	}

	FDirectoryWatcherModule& DirectoryWatcherModule = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(
		TEXT("DirectoryWatcher"));
	IDirectoryWatcher* DirectoryWatcher = DirectoryWatcherModule.Get();
	if (!DirectoryWatcher)
	{
		return;
	}

	DirectoryWatcher->RegisterDirectoryChangedCallback_Handle(Directory, Callback, InOutHandle);
	InOutDirectory = Directory;
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
		FDirectoryWatcherModule& DirectoryWatcherModule = FModuleManager::GetModuleChecked<FDirectoryWatcherModule>(
			TEXT("DirectoryWatcher"));
		if (IDirectoryWatcher* DirectoryWatcher = DirectoryWatcherModule.Get())
		{
			DirectoryWatcher->UnregisterDirectoryChangedCallback_Handle(InOutDirectory, InOutHandle);
		}
	}

	InOutHandle.Reset();
	InOutDirectory.Reset();
}
