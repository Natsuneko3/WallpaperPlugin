#pragma once

#include "CoreMinimal.h"

class SWidget;
class SWindow;

// Overwrites the current window pixels before translucent wallpaper/UI is drawn.
TSharedRef<SWidget> MakeGlassWindowBackdrop();

class FGlassWindowBackdrop
{
public:
	explicit FGlassWindowBackdrop(const TSharedRef<SWindow>& InWindow);
	~FGlassWindowBackdrop();
	bool BelongsTo(const TSharedRef<SWindow>& InWindow) const;
	bool HasWindow() const;

private:
	TWeakPtr<SWindow> Window;
	TSharedPtr<SWidget> Backdrop;
};
