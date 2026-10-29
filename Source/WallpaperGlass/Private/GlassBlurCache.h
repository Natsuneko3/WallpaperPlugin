#pragma once

#include "CoreMinimal.h"

class UTexture;

/** Shares a linear, prefiltered wallpaper between glass surfaces. */
class FGlassBlurCache
{
public:
	FGlassBlurCache();
	~FGlassBlurCache();
	FGlassBlurCache(const FGlassBlurCache&) = delete;
	FGlassBlurCache& operator=(const FGlassBlurCache&) = delete;

	/** RadiusUV is the support radius along each source texture axis; nullptr means shaders are not ready. */
	UTexture* GetBlurred(UTexture* Source, FVector2D RadiusUV, float Sigma);

private:
	struct FImpl;
	TUniquePtr<FImpl> Impl;
};
