#pragma once

#include "CoreMinimal.h"

class UMaterial;
class UTexture;

// Builds once per texture sampler type; instances share the same compiled shader.
UMaterial* CreateGlassMaterial(UTexture* Texture);

// One shared, texture-free shader for cursor-following outlines and outer glow.
UMaterial* CreateGlassHighlightMaterial();
