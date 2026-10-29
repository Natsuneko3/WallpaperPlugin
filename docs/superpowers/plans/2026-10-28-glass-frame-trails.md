# Node movement and zoom trails

User confirmed old node silhouettes remain after dragging nodes or zooming, and supplied a screenshot. This is separate from spatial blur/dispersion or the cursor's intentional smoothing.

Evidence:
- Glass shaders sample the current texture only; no history render target exists.
- The ordinary Slate window path uses ELoad unless a clear condition applies. Transparent bottom-layer drawing can therefore blend previous-frame nodes back into the current frame.
- Runtime inspection of the connected editor found global invalidation disabled, video-backed wallpaper, and brush/style alpha values of one. These values alone do not prove the final rendered background fully covers the previous frame. Do not claim a specific opacity setting is the sole cause.
- Slate ticks and paints the glass surface using the same current geometry; there is no general one-frame UV lag in this path.

Fix: add an opaque, non-hit-testable full-window base below the existing wallpaper using the public SWindow overlay API, Z order -100. Use NoBlending and an explicit black draw tint so inherited widget fade cannot retain old framebuffer contents. Preserve wallpaper opacity, video UVs, Gaussian blur, spectrum and cursor glow. Remove the base on disable/shutdown; skip native/per-pixel transparent windows. No engine changes.

Validation: reproduce old-location residue on a reused GPU render target without clearing; assert the new base removes it in the next moved-node frame while preserving translucent wallpaper. Run existing glass regression tests and inspect editor output. The original live instance requires the rebuilt module to be loaded before its specific screenshot scenario can be confirmed fixed.

Window ownership: rebind existing glass surfaces when their material tab moves between windows. Reconcile bases from attached live nodes, retaining hidden tabs but removing bases from windows whose last node was moved or destroyed. This also refreshes video UV and cursor mapping without replacing the node decoration.

Verified on 2026-10-28:
- UBT `ClientEditor Win64 Development -Module=WallpaperGlass -NoLink` succeeded against the matching `E:/sl-engine/Engine` toolchain. No engine source changes were made.
- Linked the resulting UBT objects into an isolated plugin copy under `Intermediate/GlassValidation/FrameTrailHost`; linker exit code 0. This avoids replacing the DLL held by the running user editor/debugger.
- Ran `Automation RunTests Wallpaper.Glass` in a separate D3D12 editor process using that copy. Final report: 9 successful tests, 0 failed, 0 not run; EditorIntegration contains 23 existing engine/resource-load warnings, all other tests are warning-free.
- WindowBackdrop reproduces residual pixels without the base and verifies the moved/resized frame matches an independently initialized current frame with the base. Covers wallpaper alpha 0/0.35 and inherited opacity 0.25/1.
- DecorationRestoration passes window transfer, single-surface ownership, actual old-window overlay removal, hidden-tab retention, and last-node destruction with a cached body. EditorIntegration verifies the base is attached to a real material editor window.
- Evidence: `Intermediate/GlassValidation/FrameTrailHost/ReportsFinal/index.json`, `ValidationFinal.log`, and `Saved/GlassBackdropGhostBeforeAfter.png` (left: residual node; right: corrected frame).
- Tested isolated DLL SHA256: `20DE321B2428818E70816C47B1D546DE0AF3C48024ECC16F06D5FF97A582E106`.

Pending live validation: the user chose to keep the current UE process running. Its locked plugin DLL was not replaced, so it still runs the previous implementation. After saving/closing UE and completing the normal plugin link/reload, drag nodes and zoom the same video-backed graph to confirm the original screenshot case. The independent test establishes the framebuffer-residue fix, not the exact source of incomplete coverage in the original live window.
