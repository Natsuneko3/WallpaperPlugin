# Wallpaper Glass Implementation Plan

> **For agentic workers:** Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add an independent editor module that gives material graph nodes video-backed liquid glass surfaces.

**Architecture:** WallpaperGlass decorates existing UE 5.6 material node widgets, samples the active wallpaper texture using window-relative UVs, and preserves original widgets for fallback and restoration. UI materials perform the glass shading; no engine modifications or screen capture.

**Tech Stack:** UE 5.6, Slate, MediaTexture, transient UI materials, C++.

**Spec:** Approved pure-plugin design in this conversation; user requested a new module and implementation.

## Constraints
- All edits remain inside WallpaperPlugin.
- One new Editor module, loaded PostEngineInit.
- Share the existing wallpaper resource, including video decoding.
- Preserve node interaction, restore decorated widgets on disable/shutdown.
- Refract wallpaper only; no sampling of graph wires or other nodes.

## Review focus
- Window/DPI/graph zoom mapping: analytical UV automation tests.
- Node reconstruction: check decoration ownership and rediscover rebuilt widgets.
- Resource switches/missing wallpaper: retain original widget as fallback.
- Shader sampler compatibility: use texture sample expressions with AutoSetSampleType, including media textures.
- Shutdown: remove ticker/settings, restore only widgets still owned by this module.

## Tasks
- [x] Add module descriptor/build rules, settings and UV mapping helper/tests.
- [x] Build transient glass UI materials and a Slate surface that reads active wallpaper geometry/resource.
- [x] Add material node discovery/decoration/restoration and settings registration.
- [x] Compile module, run UV and material compilation tests, review and document limitations.

## Execution notes
- Work in the current clean plugin checkout, as requested. No engine files are edited.
- Initial implementation uses five video samples for blur. Shared downsampled blur is deferred until profiling justifies it.
- Slot replacement uses the existing mutable slot's AttachWidget API through UE's const slot accessor; remove/add was rejected by a regression test because equal Z-order slots changed foreground order.
- Shader readiness uses shader-map presence and IsGameThreadShaderMapComplete. In this engine branch the legacy finalized flag remained false even with a complete shader map.
- Added a configurable linear luminance limit after inspecting the real editor screenshot with bright wallpaper.

## Verification result
- ClientEditor Win64 Development, module WallpaperGlass: build succeeded.
- Wallpaper.Glass: 4 tests passed, 0 failed, D3D12 SM6; integration run includes unrelated engine missing-resource warnings.
- Actual material editor screenshot captured and visually inspected: Intermediate/GlassValidation/Saved/GlassEditorPreview.png.
- All four tests rerun after the final appearance change. git diff --check passed.
- Actual video playback and large-graph frame-time profiling remain manual acceptance checks; media-texture shader compilation is covered.
