[English](README.md) | [繁體中文](README.zh-TW.md)

# Streamlabs Privacy Gaussian Filter

An unofficial native Windows filter that applies privacy-grade Gaussian blur to an entire Streamlabs Desktop source or a percentage-based rectangular region. It is intended to obscure text, people, and other sensitive visual information.

> [!IMPORTANT]
> This community project is not affiliated with or endorsed by Streamlabs or the OBS Project. Version 0.1.8 is verified for Streamlabs Desktop 1.21.7 with embedded OBS build **31.1.2sl19b3**. The installer fails closed on other native ABI builds.

## Download and install

1. Download `MotionFrostedGlass-Setup-0.1.8.exe` from [GitHub Releases](https://github.com/fswin040/streamlabs-privacy-gaussian-filter/releases).
2. Close Streamlabs Desktop completely.
3. Run the installer as administrator.
4. The installer checks both the embedded `obs.dll` file version and exact build marker, and continues only for `31.1.2sl19b3`.
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

- The installer reads the Windows file version and scans the embedded runtime marker before copying files.
- The plugin repeats the numeric API and exact `31.1.2sl19b3` marker check before registering the filter.

Validated configuration:

- Windows x64
- Streamlabs Desktop 1.21.7
- Embedded `obs.dll` file version 31.1.2
- Runtime OBS API 31.1.3
- Runtime build marker `31.1.2sl19b3`

Compatibility is determined by the native runtime ABI marker, not only the Streamlabs application version. Unverified markers such as `31.1.2ndi1`, `31.1.2sl22`, missing DLLs, and unreadable versions are rejected.

Streamlabs uses a fixed filter allowlist. This plugin uses the `shader_filter` compatibility source ID so it appears in the interface. Do not install it alongside another plugin using the same ID, including some OBS Shader Filter builds. Streamlabs updates may remove the plugin files; reinstall only when the installer confirms the exact verified runtime marker.

## Remove or recover

Close Streamlabs Desktop, then remove **Pure Gaussian Blur for Streamlabs** from Windows **Installed apps**.

If Streamlabs cannot start, remove these items from its internal `obs-studio-node` runtime:

- `obs-plugins\64bit\motion-frosted-glass.dll`
- `data\obs-plugins\motion-frosted-glass`

The installer does not replace `obs.dll`, the Streamlabs executable, or scene files. See [SECURITY.md](SECURITY.md) for safe issue-reporting guidance.

## Verify the download

`MotionFrostedGlass-Setup-0.1.8.exe`

```text
SHA-256 3BB6F49151289015D37C867EADCB928FC1EEFEF1CBC8FEB32D4630BBE6B7D2D7
```

PowerShell verification:

```powershell
Get-FileHash .\MotionFrostedGlass-Setup-0.1.8.exe -Algorithm SHA256
```

## Build from source

Building requires Visual Studio 2022, CMake, Windows SDK, Inno Setup 6, and a compatible Streamlabs OBS runtime. ABI headers correspond to Streamlabs fork build `31.1.2sl19b3`, commit `7fef2cce49a6e8c748e7c2da94ef3ee0dec69013`.

This repository does not redistribute Streamlabs' `obs.dll` or other proprietary runtime binaries. `compat/libobs` contains only compatibility headers. See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for source and attribution details.

## License and trademarks

This project is licensed under **GPL-2.0-or-later**; see [LICENSE](LICENSE). Streamlabs and OBS names and trademarks belong to their respective owners.
