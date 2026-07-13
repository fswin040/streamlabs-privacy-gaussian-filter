# Multilingual Documentation and OBS Core Compatibility Design

**Date:** 2026-07-13
**Status:** Approved for implementation planning

## Objective

Make English the default public documentation language while retaining a complete Traditional Chinese version, and determine installer/plugin compatibility from the embedded OBS core API family instead of the Streamlabs Desktop marketing version.

## Documentation Structure

- `README.md` is the complete English landing page shown by default on GitHub.
- `README.zh-TW.md` is a complete Traditional Chinese translation.
- Both files begin with the same language selector: `English | 繁體中文`.
- Each selector uses relative repository links so it works on branches, tags, and forks.
- Both documents contain the same sections, warnings, installation procedure, settings, compatibility rules, checksum, build instructions, licensing, and trademark disclaimer.
- Release titles and notes describe compatibility as OBS Core 31.1.x rather than binding it to Streamlabs Desktop 1.21.4. Release notes remain bilingual in one file so both languages are visible on the GitHub Release page.

## Compatibility Policy

The supported core family is **OBS 31.1.x**.

- Accept `obs.dll` versions `31.1.0` through `31.1.65535` and equivalent four-component Windows file versions.
- Accept any Streamlabs Desktop application version when its embedded `obs.dll` belongs to 31.1.x.
- Reject OBS 31.0.x, 31.2.x, all other major versions, malformed versions, missing DLLs, and DLLs whose version cannot be read.
- Patch/build components do not affect the compatibility decision.
- The public wording must say “compatible with Streamlabs Desktop builds embedding OBS Core 31.1.x,” not claim universal compatibility with every Streamlabs release.

This range balances usability with ABI safety: patch releases remain eligible while minor or major OBS changes require a new validation cycle and Release.

## Enforcement Layers

### Installer and detection scripts

The Inno Setup installer and `Detect-Streamlabs.ps1` stop checking for an exact Streamlabs Desktop version. They locate the Streamlabs runtime, read the Windows file version of `obs.dll`, parse its numeric components, and allow installation only when major is 31 and minor is 1.

Error messages show the detected Streamlabs version when available and the detected OBS core version. A missing or unreadable version fails closed.

### Plugin runtime

During `obs_module_load`, the plugin calls the OBS runtime version API and checks major 31/minor 1 before registering the filter. An incompatible runtime produces a clear log error and returns `false`, leaving the filter unregistered. This protects manual installations and cases where Streamlabs updates after installation.

The runtime check uses only OBS version functions already present in the supported ABI and performs no graphics work before compatibility is established.

## Tests

Tests are added before production changes and must demonstrate the expected failure first.

- Version policy unit tests accept `31.1.0`, `31.1.2`, `31.1.3`, and `31.1.99`.
- Unit tests reject `31.0.99`, `31.2.0`, `32.0.0`, empty values, and malformed strings.
- Installer source tests verify that exact Streamlabs `1.21.4` gating has been removed and the 31.1.x major/minor check exists.
- PowerShell detection is exercised against factored version-policy logic with accepted and rejected inputs.
- Module-load integration confirms the plugin registers on the installed OBS 31.1.x runtime.
- Documentation checks confirm both README files exist, contain reciprocal language links, have matching required sections, and state the same compatibility range and installer checksum.
- The installer is rebuilt, its payload test is rerun, and its new SHA-256 is written into both README files and the Release notes.

## Release Update

- Keep plugin version `0.1.6`; this change adjusts distribution policy and documentation without changing the Gaussian renderer.
- Replace the existing `v0.1.6-beta` installer asset and Release notes so users do not encounter two different compatibility policies under the same tag.
- Verify the uploaded asset digest against the locally rebuilt installer.

## Out of Scope

- Supporting OBS 31.0.x, 31.2.x, or OBS 32.x.
- Automatically downloading different plugin builds.
- Code signing the installer.
- Changing blur rendering, strength, quality, or region behavior.
