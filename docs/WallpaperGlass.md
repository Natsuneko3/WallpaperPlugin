# Wallpaper Glass

UE 5.6 editor module for video-backed material graph nodes. All source changes are inside this plugin.

## Use

Build the editor with the updated plugin and restart it so that the new `WallpaperGlass` module is discovered. Open **Editor Preferences → Plugins → Wallpaper Glass**. `Enabled` defaults to true. Configure the wallpaper/video through the existing Wallpaper settings, then open a material graph.

- **Corner Radius / Edge Width:** rounded silhouette and glass edge thickness, in node-local Slate units.
- **Refraction:** edge displacement in node-local Slate units.
- **Transmission:** uniform light transmission; lower it for dark glass and readable white labels over bright backgrounds.
- **Absorption Color:** multiplicative glass color. The default preset uses purple, linear RGBA `(0.470093, 0.278944, 0.473958, 1)`.
- **Glass Opacity:** coverage of the glass surface, independent of the wallpaper's opacity.
- **Black Node Text:** disabled by default. When enabled, titles, labels and inactive numeric text inside glass nodes display in black; pin icons and other editor text retain their colors. Focused editors or fields with a text selection retain native colors so the caret and selection remain readable. Turning this off restores the original text appearance. Native widgets and color bindings are preserved.
- **Highlight Strength:** subtle glass reflection, independent of the mouse-following colored outline.
- **Scattering Strength / Scattering Radius:** blend and radius of a normalized eight-direction diffusion filter. It softens background detail without adding white or changing a uniform background's hue.
- **Show Undistorted Video:** disables refraction, blur, scattering, dispersion, absorption and highlights, with full surface coverage, to check raw video UV alignment.

The default preset matches the selected editor configuration: Corner Radius 16, Edge Width 8.8, Refraction 15, Transmission 1, Highlight Strength 1 and Glass Opacity 1. Scattering Strength and Radius both default to zero. Existing saved user preferences still take precedence over class defaults.

### Spectral dispersion and Gaussian blur

The rim now samples nine offset wavelength bands, from violet/blue through cyan, green, yellow, orange and red. Nonnegative RGB band weights are normalized per channel: a constant-color background stays that color. This is a stylized spectral approximation using the video RGB texture. Color fringes come from displaced background detail, so they are most visible on bright/dark boundaries; a uniform background does not gain an artificial rainbow tint. Scattering no longer reduces dispersion strength.

| Preference | Default | Range | Effect |
| --- | --- | --- | --- |
| Dispersion Radius | 15 | 0–48 | Maximum sample displacement in either direction, in local Slate units; zero disables dispersion |
| Dispersion Strength | 1 | 0–1 | Blend toward the spectral result; zero disables dispersion |
| Dispersion Edge Width | 96 | 1–96 | Width of the spectral band inside the node, independent of glass Edge Width |
| Dispersion Edge Power | 1.2 | 0.25–4 | Higher values concentrate separation at the outer edge |
| Blur Radius | 24 | 0–32 | Support radius of the shared, prefiltered Gaussian blur |
| Gaussian Strength | 1 | 0–1 | Center blur blend, independent of scattering |
| Gaussian Sigma | 0.6 | 0.15–1.5 | Gaussian standard deviation as a fraction of Blur Radius; larger values spread more weight outward |
| Gaussian Edge Retention | 1 | 0–1 | Fraction of Gaussian strength retained at the curved rim; 1 keeps the entire surface frosted |

Parameters update live under **Spectral Dispersion** and **Gaussian Blur**. For an evenly frosted surface, use Blur Radius 24, Gaussian Strength 1, Sigma 0.6, Gaussian Edge Retention 1 and Glass Opacity 1. Lower edge retention deliberately restores sharp background at the rim; lower opacity reveals the original background beneath the surface. Text and pins remain crisp because they are painted above the glass.

The wallpaper is prefiltered into a shared linear texture using horizontal and vertical Gaussian passes. Both scattering and spectral dispersion sample this same blurred source, so enabling them cannot replace the blur with sharp wallpaper. Mip prefiltering covers the space between samples at large radii, avoiding the old sparse 5×5 kernel's repeated details and periodic aliasing.

Existing saved `DispersionRadius` and `BlurRadius` values are preserved. If the old radius was zero, set Dispersion Radius to 15 or reset that preference; updating the plugin does not overwrite saved preferences. Zero Gaussian radius or strength disables Gaussian blur, while the separate scattering control remains active.

### Cursor highlight

**Cursor Highlight** is enabled by default. A soft blue outline brightens near the pointer, with a halo outside the node. Default linear RGBA is `(0.005450, 0.251048, 1, 1)` with Strength 1. It uses node-local cursor distance, including when the pointer is outside the node; moving between opposite sides moves the bright area smoothly.

- **Cursor Highlight Color / Strength / Width:** outline color, intensity and thickness. Color alpha also scales the effect.
- **Cursor Highlight Radius:** pointer influence radius; smaller values create a tighter hotspot.
- **Cursor Highlight Ambient:** faint outline remaining far from the pointer. Set to zero for proximity-only lighting.
- **Cursor Glow Radius / Strength:** spread and intensity of the soft halo; either set to zero removes the halo.
- **Cursor Follow Speed:** response speed; higher values follow more quickly.

Widths and radii are in node-local Slate units and scale with graph zoom. These preferences apply immediately. Disable **Cursor Highlight**, or set its strength to zero, to remove this layer. The diagnostic video view also suppresses it. The outline uses a separate additive material with no texture sampling; the glow adds light only near the boundary. Node layout, pins and hit testing are unchanged.

The wallpaper and nodes share the same texture and video decoder. Each node has its own dynamic material instance; compiled parent materials are cached by sampler type. Geometry changes update window-relative UVs. No CPU video readback, backbuffer capture, global shaders, or engine patches are used.

Unchanged ordinary image assets reuse their blurred results across frames. Resource replacement, size, resident mip and editor texture revision changes invalidate that result. Videos, render targets and transient textures remain live. Blur targets and material instances are reused across frames when graph zoom changes, while targets already used by the current frame are protected from being overwritten.

Ordinary windows containing decorated nodes also receive an opaque black layer below the existing wallpaper. Slate normally loads the previous window backbuffer; the base layer overwrites it before translucent content is composited, preventing old node silhouettes from accumulating after dragging or zooming. Wallpaper transparency still blends over this stable base. The layer is per-window, does not receive input, and is removed when its last decorated node is closed or moved elsewhere, glass is disabled, or the module shuts down. Hidden attached tabs retain their base. Moving a tab between windows also rebinds its video UV and cursor coordinates. Native OS-bordered and per-pixel transparent windows are excluded. This addresses framebuffer history; intentional spectral offsets and cursor smoothing are separate effects.

## Compatibility and limits

The adapter targets the UE 5.6 standard material-node body layout. It supports standard material, Custom, output and composite nodes when they use that layout. Unrecognized layouts remain unchanged. Rebuilt nodes are rediscovered, and disabling the effect restores original background widgets and title brushes.

The effect refracts the wallpaper only. Graph wires, grid lines and other nodes are not part of the sampled texture. The current wallpaper renderer stretches its image over the window; future cover/contain modes must update the UV transform too. Native OS-bordered windows fall back to the original node appearance. Partially transparent wallpaper may show different compositing through the glass; full background opacity is recommended for visual matching.

Each glass material has 36 samples: original and blurred versions of the center, eight diffuse offsets and nine spectral offsets. The shared blur uses a source copy/mip chain and two 33-sample separable passes; its intermediate resolution decreases with the sampling footprint. Equal source/radius/sigma combinations share the result across nodes. No CPU video readback or extra video decoder is used. Fixed sample counts do not imply that zero strengths eliminate GPU work. The cursor outline adds one texture-free material draw per visible node. Its expanded quad preserves graph clipping; like native node shadows, the halo disappears once the node body leaves the graph's visible area. Large-graph frame cost has not been profiled.

Older `Tint`, `TintStrength` and `BackgroundLuminanceLimit` config entries are ignored. Their forced color cast and hard luminance clipping have been replaced by the neutral transmission/absorption controls. No gamma conversion is added: the wallpaper and glass use UE's existing UI texture sampling and Slate output conversion.

## Verification

Automation prefix: `Wallpaper.Glass`.

- `UVMapping`: negative window origins, 100/150/200% DPI, 25/100/200% graph scale, zero-size rejection.
- `MaterialCompilation`: actual image and media texture shader compilation with a rendering RHI.
- `ColorAndScattering`: GPU readback against a direct texture reference, tonal ramp preservation, uniform-color scattering invariance, and high-contrast edge diffusion.
- `Gaussian`: constant-color preservation, zero-strength/radius bypass, high-frequency contrast reduction, and sigma-controlled step diffusion. Saves `Saved/GlassGaussianFrequency.png` and `Saved/GlassGaussianStep.png`.
- `FrostedBlur`: the old implementation fails at radius 16 and 32 on period-eight stripes. The fixed path covers 18 radius/period/phase combinations and six edge scattering/dispersion combinations, checking mean color, RMS and peak-to-peak contrast. Saves `Saved/GlassFrostedBlur.png`.
- `SpectralDispersion`: constant-color preservation, multiple spectral fringe colors, visibility with full scattering, an unaffected center, and independent zero-strength/radius bypass with other filters active. Saves `Saved/GlassSpectrum.png` (left: disabled; middle: spectrum; right: spectrum plus full scattering).
- `CursorHighlight`: GPU readback for left/right hotspot movement, distance fading and ambient light, an exterior halo with a black outer boundary, an unchanged center, zero-strength output, and aspect-ratio independence. Saves `Saved/GlassHighlightCursorLeft.png` and `Saved/GlassHighlightCursorRight.png`.
- `DecorationRestoration`: foreground order, padding, original widget restoration, node rebuild ownership, window reassignment without duplicate decoration, hidden-tab retention, and old/empty-window backdrop removal.
- `WindowBackdrop`: draws successive moved/resized-node frames into the same GPU render target without clearing, demonstrates residual pixels without the base, and checks complete replacement with it. Covers transparent wallpaper and inherited opacity; saves `Saved/GlassBackdropGhostBeforeAfter.png` (left: residue; right: stable background).
- `EditorIntegration`: opens a transient material editor, verifies real node decoration, window backdrop attachment and both glass/highlight material readiness, and saves `Saved/GlassEditorPreview.png` when screenshot capture is available.

Use a disposable verification project for editor integration tests. These tests require a rendering RHI; do not pass `-NullRHI`.
