[English](README.md) | [繁體中文](README.zh-TW.md)

# Streamlabs Privacy Gaussian Filter

An unofficial native Windows filter that applies privacy-grade Gaussian blur to an entire Streamlabs Desktop source or a percentage-based rectangular region. It is intended to obscure text, people, and other sensitive visual information.

> [!IMPORTANT]
> This community project is not affiliated with or endorsed by Streamlabs or the OBS Project. It supports Streamlabs Desktop builds whose embedded **OBS Core API is 31.1.x**. Streamlabs updates can remove the plugin or change internal behavior even when the core version remains compatible.

## Download and install

1. Download `MotionFrostedGlass-Setup-0.1.6.exe` from [GitHub Releases](https://github.com/fswin040/streamlabs-privacy-gaussian-filter/releases).
2. Close Streamlabs Desktop completely.
3. Run the installer as administrator.
4. The installer checks the embedded `obs.dll` file version and continues only for OBS Core 31.1.x.
5. Open a source's Filters window in Streamlabs and add **Pure Gaussian Blur**. Some Streamlabs interfaces may display **Shader** because of the compatibility source ID.

The installer is unsigned, so Windows SmartScreen may show a warning. Verify the SHA-256 below before deciding whether to select **More info → Run anyway**.

## Features

- Full-source or custom rectangular blur region
- Percentage-based region position and size
- Adjustable blur strength
- Low, medium, and high quality modes
- Reset to defaults
- Zero-strength rendering bypass
- Region-of-interest processing with a safe blur margin to reduce GPU work

This release focuses only on true Gaussian blur. It does not include animation, glass tint, colored lighting, borders, or grain.

## Compatibility and safety

The installer and plugin use two separate checks:

- The installer reads the Windows file version of Streamlabs' embedded `obs.dll` and accepts major 31, minor 1, regardless of patch/build or Streamlabs Desktop application version.
- The plugin checks the loaded OBS Core API again before registering the filter and refuses to load outside API 31.1.x.

Validated configuration:

- Windows x64
- Streamlabs Desktop 1.21.4
- Embedded `obs.dll` file version 31.1.2
- Runtime OBS API 31.1.3

This is a validated example, not an exact Streamlabs application-version requirement. OBS Core 31.0.x, 31.2.x, 32.x, missing DLLs, and unreadable versions are rejected.

Streamlabs uses a fixed filter allowlist. This plugin uses the `shader_filter` compatibility source ID so it appears in the interface. Do not install it alongside another plugin using the same ID, including some OBS Shader Filter builds. Streamlabs updates may remove the plugin files; reinstall only when the updated build still embeds OBS Core 31.1.x.

## Remove or recover

Close Streamlabs Desktop, then remove **Pure Gaussian Blur for Streamlabs** from Windows **Installed apps**.

If Streamlabs cannot start, remove these items from its internal `obs-studio-node` runtime:

- `obs-plugins\64bit\motion-frosted-glass.dll`
- `data\obs-plugins\motion-frosted-glass`

The installer does not replace `obs.dll`, the Streamlabs executable, or scene files. See [SECURITY.md](SECURITY.md) for safe issue-reporting guidance.

## Verify the download

`MotionFrostedGlass-Setup-0.1.6.exe`

```text
SHA-256 78107D61E82461FD79353DD307A8A8921B4C702B9C50B65F5DF6D1E7416BCE70
```

PowerShell verification:

```powershell
Get-FileHash .\MotionFrostedGlass-Setup-0.1.6.exe -Algorithm SHA256
```

## Build from source

Building requires Visual Studio 2022, CMake, Windows SDK, Inno Setup 6, and a compatible Streamlabs OBS runtime. ABI headers correspond to the Streamlabs `obs-studio` fork tag `31.1.2ndi1`, commit `32985a8f9684035bfcfbea781046c88dba617024`.

This repository does not redistribute Streamlabs' `obs.dll` or other proprietary runtime binaries. `compat/libobs` contains only compatibility headers. See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for source and attribution details.

## License and trademarks

This project is licensed under **GPL-2.0-or-later**; see [LICENSE](LICENSE). Streamlabs and OBS names and trademarks belong to their respective owners.
