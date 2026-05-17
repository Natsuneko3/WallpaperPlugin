#pragma once

#include "CoreMinimal.h"
#include "IDirectoryWatcher.h"

class FWallpaperDirectoryWatcher
{
public:
	static void Watch(
		FDelegateHandle& InOutHandle,
		FString& InOutDirectory,
		const FString& Directory,
		const IDirectoryWatcher::FDirectoryChanged& Callback);

	static void Stop(FDelegateHandle& InOutHandle, FString& InOutDirectory);
};
