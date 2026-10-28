#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WallpaperGlassSettings.generated.h"

UCLASS(Config=EditorPerProjectUserSettings)
class UWallpaperGlassSettings : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(Config, EditAnywhere, Category="Glass")
	bool bEnabled = true;

	// Editable fields retain their native colors while focused or showing a text selection.
	UPROPERTY(Config, EditAnywhere, Category="Text")
	bool bBlackNodeText = false;

	UPROPERTY(Config, EditAnywhere, Category="Glass", meta=(ClampMin="0", ClampMax="40"))
	float CornerRadius = 16.0f;

	UPROPERTY(Config, EditAnywhere, Category="Glass", meta=(ClampMin="1", ClampMax="40"))
	float EdgeWidth = 8.8f;

	UPROPERTY(Config, EditAnywhere, Category="Glass", meta=(ClampMin="0", ClampMax="20"))
	float Refraction = 15.0f;

	// Support radius of the 5x5 Gaussian kernel in node-local Slate units.
	UPROPERTY(Config, EditAnywhere, Category="Gaussian Blur", meta=(ClampMin="0", ClampMax="32"))
	float BlurRadius = 3.0f;

	UPROPERTY(Config, EditAnywhere, Category="Gaussian Blur", meta=(ClampMin="0", ClampMax="1"))
	float GaussianStrength = 1.0f;

	// Sigma relative to Blur Radius. Higher values spread weight toward the outer taps.
	UPROPERTY(Config, EditAnywhere, Category="Gaussian Blur", meta=(ClampMin="0.15", ClampMax="1.5"))
	float GaussianSigma = 1.5f;

	// Fraction of Gaussian blur retained at the curved edge; the center uses full strength.
	UPROPERTY(Config, EditAnywhere, Category="Gaussian Blur", meta=(ClampMin="0", ClampMax="1"))
	float GaussianEdgeRetention = 0.15f;

	UPROPERTY(Config, EditAnywhere, Category="Glass", meta=(ClampMin="0", ClampMax="1"))
	float Transmission = 1.0f;

	UPROPERTY(Config, EditAnywhere, Category="Glass")
	FLinearColor AbsorptionColor = FLinearColor(0.470093f, 0.278944f, 0.473958f, 1.0f);

	UPROPERTY(Config, EditAnywhere, Category="Glass", meta=(ClampMin="0", ClampMax="2"))
	float HighlightStrength = 1.0f;

	UPROPERTY(Config, EditAnywhere, Category="Glass", meta=(ClampMin="0", ClampMax="1"))
	float GlassOpacity = 1.0f;

	UPROPERTY(Config, EditAnywhere, Category="Cursor Highlight")
	bool bCursorHighlight = true;

	UPROPERTY(Config, EditAnywhere, Category="Cursor Highlight")
	FLinearColor CursorHighlightColor = FLinearColor(0.005450f, 0.251048f, 1.0f, 1.0f);

	UPROPERTY(Config, EditAnywhere, Category="Cursor Highlight", meta=(ClampMin="0", ClampMax="4"))
	float CursorHighlightStrength = 1.0f;

	UPROPERTY(Config, EditAnywhere, Category="Cursor Highlight", meta=(ClampMin="0.5", ClampMax="6"))
	float CursorHighlightWidth = 1.4f;

	// Mouse influence radius in node-local Slate units, including outside the node.
	UPROPERTY(Config, EditAnywhere, Category="Cursor Highlight", meta=(ClampMin="16", ClampMax="600"))
	float CursorHighlightRadius = 180.0f;

	// Faint outline that remains when the mouse is far away; zero gives proximity-only light.
	UPROPERTY(Config, EditAnywhere, Category="Cursor Highlight", meta=(ClampMin="0", ClampMax="1"))
	float CursorHighlightAmbient = 0.12f;

	UPROPERTY(Config, EditAnywhere, Category="Cursor Highlight", meta=(ClampMin="0", ClampMax="32"))
	float CursorGlowRadius = 10.0f;

	UPROPERTY(Config, EditAnywhere, Category="Cursor Highlight", meta=(ClampMin="0", ClampMax="2"))
	float CursorGlowStrength = 0.3f;

	UPROPERTY(Config, EditAnywhere, Category="Cursor Highlight", meta=(ClampMin="1", ClampMax="40"))
	float CursorFollowSpeed = 12.0f;

	// A normalized spatial filter diffuses background light without adding white or changing hue.
	UPROPERTY(Config, EditAnywhere, Category="Scattering", meta=(ClampMin="0", ClampMax="1"))
	float ScatteringStrength = 0.0f;

	UPROPERTY(Config, EditAnywhere, Category="Scattering", meta=(ClampMin="0", ClampMax="32"))
	float ScatteringRadius = 0.0f;

	// Maximum offset on either side of the refracted UV, in node-local Slate units.
	UPROPERTY(Config, EditAnywhere, Category="Spectral Dispersion", meta=(ClampMin="0", ClampMax="48"))
	float DispersionRadius = 15.0f;

	// Independent of scattering: strong diffusion does not suppress spectral separation.
	UPROPERTY(Config, EditAnywhere, Category="Spectral Dispersion", meta=(ClampMin="0", ClampMax="1"))
	float DispersionStrength = 1.0f;

	UPROPERTY(Config, EditAnywhere, Category="Spectral Dispersion", meta=(ClampMin="1", ClampMax="96"))
	float DispersionEdgeWidth = 96.0f;

	// Larger values concentrate the spectrum closer to the perimeter.
	UPROPERTY(Config, EditAnywhere, Category="Spectral Dispersion", meta=(ClampMin="0.25", ClampMax="4"))
	float DispersionEdgePower = 1.2f;

	// Useful for checking that the video is continuous across the node boundary.
	UPROPERTY(Config, EditAnywhere, Category="Diagnostics")
	bool bShowUndistortedVideo = false;
};
