# Glass color and scattering correction

User request: fix unnatural glass colors in the supplied screenshot and add scattering; retain pure-plugin implementation.

Confirmed causes: fixed blue interpolation lifts/recolors shadows; per-pixel luminance cap collapses highlights; reusing wallpaper alpha as glass coverage double-composites the source; source RGB tint was ignored. Both background and glass already use the same UE UI sampling/gamma path, so no extra gamma conversion is appropriate.

Implementation:
- Replace color cast/clipping with neutral absorption times uniform transmission.
- Separate glass opacity from wallpaper opacity and honor source RGB tint.
- Add normalized spatial scattering with adjustable radius/strength.
- Add independent optional edge dispersion, disabled by default.
- Preserve geometry, widget ownership, input and fallback behavior.

Verification:
- Rebuild WallpaperGlass using ClientEditor Win64 Development.
- Run Wallpaper.Glass including material compile, UV, widget restoration and editor integration tests.
- Add GPU image tests for neutral color, bright/dark tonal preservation, constant-color scattering invariance and spatial contrast reduction.
- Inspect real-editor screenshot and document remaining performance/transparent-background limitations.

Results: module build succeeded; all 5 Wallpaper.Glass tests passed on D3D12 SM6 (one integration test includes existing engine resource warnings). GPU readback verified direct-texture color agreement, unclipped grayscale tones, constant-color invariance and edge diffusion. Independent review caught negative sampling weights in the optional dispersion blend; replaced with a convex blend. A zero scattering radius now disables its contribution. Real editor screenshot inspected. No engine source changes.
