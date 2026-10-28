#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"

class SWallpaperGlassSurface;

// Keep native text widgets and their bound attributes intact; only wrap painting.
class FGlassTextDecoration
{
public:
	~FGlassTextDecoration();
	void Update(const TSharedRef<SWidget>& Root, const TSharedRef<SWallpaperGlassSurface>& Surface);
	void Restore();

private:
	struct FEntry
	{
		TWeakPtr<SWidget> Owner;
		TSharedPtr<SWidget> Original;
		TSharedPtr<SWidget> Wrapper;
	};
	void Visit(const TSharedRef<SWidget>& Owner, const TSharedRef<SWallpaperGlassSurface>& Surface);
	TArray<FEntry> Entries;
};
