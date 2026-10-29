# Cursor-following glass outline

User request: luminous node perimeter with a highlight that follows mouse position, like the supplied reference. Keep the implementation entirely within the plugin and preserve the existing video glass/color/scattering work.

The existing glass rim is biased toward the top left and clamps the cursor. It cannot create an exterior halo because its quad and alpha stop at the body boundary.

Implementation:
- Add a shared, texture-free additive UI material for a rounded outline and soft halo.
- Use unclamped node-local cursor coordinates and exponential smoothing with first-frame initialization.
- Calculate proximity in local Slate units so rectangular nodes have circular hotspots.
- Paint an expanded quad using the existing body geometry, preserving layout, foreground order, hit testing, and graph clipping.
- Expose color, width, strength, influence radius, ambient rim, glow radius/strength and follow speed in Editor Preferences.
- Keep the existing subtle glass reflection independent; diagnostics and feature disable suppress the new layer.

Verification: rebuild the module; run all Wallpaper.Glass automation tests, including GPU checks for hotspot movement, distance fade, exterior halo, unchanged center and zero intensity; inspect offscreen material-editor and highlight previews. No engine modifications.

Results: ClientEditor Win64 Development module build succeeded. All 6 Wallpaper.Glass tests passed on D3D12 SM6 (5 clean passes, 1 editor integration pass with existing engine resource warnings). The first shader run caught the reserved HLSL identifier `line`; renamed it to `rimBand` and reran the suite successfully. GPU previews show the hotspot switching from left to right, exterior glow and an unchanged center. The normal editor screenshot shows the cyan outline alongside the native yellow selection border. Independent review checked additive blending, coordinate conversion, clipping, layer order and disable paths. Large-graph frame cost has not been profiled.
