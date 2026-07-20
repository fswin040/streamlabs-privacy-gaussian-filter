# Streamlabs `31.1.2sl19b3` Compatibility Design

**Date:** 2026-07-20  
**Status:** Approved for specification review

## Objective

Restore the Pure Gaussian Blur filter on Streamlabs Desktop 1.21.7, whose embedded OBS runtime identifies itself as `31.1.2sl19b3`, without modifying users' scenes or silently installing on an unverified ABI.

This design supersedes the earlier assumption that every OBS Core `31.1.x` Streamlabs build shares one native module ABI.

## Root Cause

The released plugin was compiled with the `31.1.2ndi1` Streamlabs fork headers. Between that build and `31.1.2sl19b3`, Streamlabs moved the `message` and `get_messages` members of `obs_source_info` from the middle of the structure to its append-only tail. The old layout shifts all following callback offsets, including `video_render`.

On Streamlabs Desktop 1.21.7 the module therefore loads, its settings callbacks work, and existing `shader_filter` instances remain visible, but the Gaussian renderer is not called through the expected callback slot. The installed shaders themselves still compile successfully.

## Supported Runtime

Version 0.1.7-beta targets the verified runtime below:

- Streamlabs Desktop 1.21.7 on Windows x64
- OBS runtime string `31.1.2sl19b3`
- Runtime numeric API `31.1.3`
- Current `sl19b3` `obs_source_info` layout

The installer must fail closed for unknown Streamlabs OBS builds. A numeric Windows file version of `31.1.x` alone is no longer sufficient evidence of compatibility.

## Implementation

### ABI headers and module registration

- Replace the compatibility `obs-source.h` layout with the verified `31.1.2sl19b3` definition.
- Retain the `shader_filter` compatibility ID required by the Streamlabs filter allowlist.
- Keep the existing Gaussian renderer, settings schema, locale keys, and scene serialization unchanged.
- Update the runtime policy so the module registers only when the runtime build string matches the verified `31.1.2sl19b3` ABI generation.

### Installer detection

- Detect the embedded `obs.dll` and read both its numeric version and embedded runtime build marker.
- Install only when the numeric API family remains 31.1.x and the build marker is `31.1.2sl19b3`.
- Show a clear bilingual unsupported-build error for missing, unreadable, or unknown runtimes.
- Preserve the existing installation paths and uninstall behavior.

### Existing user data

The installer replaces only the plugin DLL and its data directory. It does not edit scene collections. Existing filters retain their type ID, enabled state, region, strength, and quality settings.

## Test-First Verification

Before changing production code, retain and run the current image-filter integration test against Streamlabs 1.21.7 as the failing regression case. The observed failure is zero renderer callbacks and zero Gaussian passes.

After rebuilding, the same test must prove:

- the image source and `shader_filter` instance are created;
- the filter is attached and enabled;
- renderer callback count is non-zero;
- Gaussian pass count meets the expected per-frame minimum;
- no `invalid param` or `No vertex shader specified` errors occur.

Additional tests must cover:

- ABI build-marker acceptance and rejection;
- continued settings normalization and Gaussian calculations;
- shader compilation against the installed Direct3D 11 runtime;
- installer payload and unsupported-runtime behavior;
- installation into the actual Streamlabs 1.21.7 runtime followed by a second image-render integration run.

## Release

- Increment the project and installer version to `0.1.7`.
- Produce `MotionFrostedGlass-Setup-0.1.7.exe`.
- Update English and Traditional Chinese documentation to identify `31.1.2sl19b3` as the verified native ABI, rather than claiming all 31.1.x builds.
- Add bilingual v0.1.7-beta release notes and publish only after local installation and rendering verification pass.
- Keep v0.1.6-beta available as historical material, but mark its broad 31.1.x compatibility claim as superseded.

## Failure Handling

- An unsupported runtime must be rejected before module registration or file replacement.
- Shader or texture initialization failure must bypass the filter safely and emit one actionable log message rather than crashing Streamlabs.
- Installer validation failure must leave an existing working installation untouched.

## Out of Scope

- A single adaptive DLL containing both incompatible `obs_source_info` layouts.
- Support for unverified Streamlabs builds or OBS 31.0.x, 31.2.x, and 32.x.
- Automatic background repair after future Streamlabs updates.
- Changes to blur appearance, strength range, quality modes, or region controls.
