# Spectral dispersion and Gaussian center blur

Requested: replace weak RGB splitting with multisample spectral color offsets, add Gaussian blur in the center, and expose tuning controls. Work in E:/PJW/Project/Client/Plugins/WallpaperPlugin, entirely inside the plugin.

Design:
- Nine independently offset wavelength bands with nonnegative RGB weights, normalized per channel. This is a stylized spectral approximation in RGB, not a physical spectral renderer. Uniform colors remain unchanged.
- Independent dispersion radius, strength, edge width and edge falloff. Strong scattering must no longer suppress dispersion. Zero strength or radius bypasses it exactly.
- A normalized 5x5 Gaussian kernel with adjustable support radius, sigma, blend strength and blur retained at the curved edge.
- Preserve cursor highlight, native selection border, UV mapping and MediaTexture sampler compatibility. No extra gamma conversion or fixed luminance clamp.
- Keep existing scattering controls. The material uses 42 samples (25 Gaussian including center, 8 scattering, 9 spectral); no additional video decoder or CPU readback.

Verification: compile the current workspace module, run all Wallpaper.Glass GPU and editor tests in a disposable host that points to this workspace, inspect saved spectral/Gaussian previews, and document performance and saved-setting caveats.

Results:
- Built both WallpaperGlass and its WallPaper dependency using E:/PJW/UGSTest/Engine, whose BuildId and checked engine DLL/import-library hashes match the installed E:/PJW/Project/Engine. No manual BuildId override and no engine source changes. An earlier sl-engine build-graph-only check was superseded by this matching toolchain build.
- The disposable host references the current E:/PJW/Project/Client/Plugins/WallpaperPlugin directory. All eight tests passed on D3D12 SM6: seven clean passes and one editor integration pass with existing engine resource warnings; zero failures.
- Inspected GlassSpectrum.png (disabled / spectral / spectral with full scattering), GlassGaussianStep.png (sharp / narrow sigma / broad sigma), and the real material editor screenshot.
- Gaussian weights factor into two one-dimensional kernels, retaining the exact 25-point result while using only two distinct exponential evaluations. Independent mathematical review passed.
- Uniform-color invariance, center isolation, zero-parameter bypass, multicolor fringes, scattering compatibility and Gaussian attenuation were checked by GPU readback. Large-graph frame cost is not profiled; the increased sampling cost and sparse-kernel large-radius limit are documented.
