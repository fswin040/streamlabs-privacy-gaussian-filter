# Third-party notices / 第三方聲明

## OBS / Streamlabs OBS fork compatibility headers

Files under `compat/libobs/` are compatibility headers derived from the Streamlabs `obs-studio` fork, tag `31.1.2ndi1`, commit `32985a8f9684035bfcfbea781046c88dba617024`.

- Upstream: <https://github.com/stream-labs/obs-studio>
- License: GNU General Public License version 2 or later

The project does not redistribute Streamlabs Desktop, `obs.dll`, or other Streamlabs runtime binaries.

## Inno Setup

The Windows installer is built with Inno Setup 6. Inno Setup is Copyright (C) 1997-2026 Jordan Russell and Copyright (C) 2000-2026 Martijn Laan, distributed under its own permissive license.

- Project: <https://jrsoftware.org/isinfo.php>
- License: <https://jrsoftware.org/files/is/license.txt>

`installer/Languages/ChineseTraditional.isl` is a contributed Traditional Chinese translation whose header credits Enfeng Tsao and Samuel Lee. It is retained with its original attribution for the installer UI.

## Microsoft Direct3D / Windows SDK

The source uses Direct3D shader compiler interfaces supplied by the Windows SDK. No Windows SDK binary is committed to this repository.
