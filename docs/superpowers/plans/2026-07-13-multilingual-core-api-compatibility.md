# Multilingual OBS Core Compatibility Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Publish English-first bilingual documentation and permit installation/loading on Streamlabs Desktop builds embedding OBS Core 31.1.x, independent of the Streamlabs application patch version.

**Architecture:** A small pure C++ version-policy unit validates the encoded runtime API before filter registration. PowerShell detection and Inno Setup independently validate the Windows `obs.dll` file-version major/minor while failing closed on missing or unreadable versions. English and Traditional Chinese README files mirror the same public contract and checksum.

**Tech Stack:** C++17, libobs 31.1.x API, PowerShell 7, Inno Setup 6, CMake/CTest, Git/GitHub CLI.

## Global Constraints

- Accept only OBS Core major 31 and minor 1; ignore patch/build components.
- Do not require an exact Streamlabs Desktop application version.
- Fail closed for missing, unreadable, or malformed OBS versions.
- Keep plugin and installer version `0.1.6`.
- Do not alter Gaussian rendering, quality, strength, or region behavior.
- `README.md` is English; `README.zh-TW.md` is Traditional Chinese; both begin with reciprocal language links.

---

### Task 1: Pure OBS runtime API compatibility policy

**Files:**
- Create: `src/obs-version-policy.hpp`
- Create: `src/obs-version-policy.cpp`
- Create: `tests/obs-version-policy-test.cpp`
- Modify: `CMakeLists.txt`
- Modify: `src/plugin-main.cpp`

**Interfaces:**
- Produces: `bool motion_frosted_obs_version_supported(uint32_t version) noexcept`
- Consumes: OBS semantic version encoding `(major << 24) | (minor << 16) | patch`

- [ ] **Step 1: Write the failing C++ policy test**

Create table-driven assertions for encoded versions 31.1.0, 31.1.2, 31.1.3, and 31.1.99 returning true; 31.0.99, 31.2.0, and 32.0.0 returning false.

- [ ] **Step 2: Build/run the test and verify RED**

Run the MSVC compile command used by the existing native tests. Expected: compilation fails because `obs-version-policy.hpp` and its function do not exist.

- [ ] **Step 3: Implement minimal policy and runtime guard**

Decode major from bits 24–31 and minor from bits 16–23. In `obs_module_load`, call `obs_get_version()` before `obs_register_source`; log `obs_get_version_string()` and return false when unsupported.

- [ ] **Step 4: Build and verify GREEN**

Run the policy test, existing filter settings test, Gaussian test, shader contract test, D3D11 shader runtime test, and module-load test. Expected: all exit 0; module log includes a successful 31.1.x load.

- [ ] **Step 5: Commit the runtime policy**

Commit `src/obs-version-policy.*`, `tests/obs-version-policy-test.cpp`, `src/plugin-main.cpp`, and `CMakeLists.txt` with message `Support OBS Core 31.1.x runtime family`.

### Task 2: PowerShell and installer compatibility enforcement

**Files:**
- Create: `scripts/ObsVersionPolicy.ps1`
- Create: `scripts/Test-ObsVersionPolicy.ps1`
- Modify: `scripts/Detect-Streamlabs.ps1`
- Modify: `scripts/Test-SetupPayload.ps1`
- Modify: `installer/MotionFrostedGlass.iss`

**Interfaces:**
- Produces: `Test-SupportedObsVersion([string] $Version) -> [bool]`
- Installer consumes `GetVersionComponents(obs.dll, Major, Minor, Revision, Build)` and accepts only `Major = 31` and `Minor = 1`.

- [ ] **Step 1: Write failing PowerShell and installer-source tests**

Assert accepted strings `31.1.0`, `31.1.2.0`, `31.1.3`, `31.1.99.7`; reject `31.0.99`, `31.2.0`, `32.0.0`, empty, and malformed values. Update setup-source checks to require major/minor component checks and reject exact Streamlabs or exact OBS patch comparisons.

- [ ] **Step 2: Run tests and verify RED**

Run `pwsh -File scripts/Test-ObsVersionPolicy.ps1` and `pwsh -File scripts/Test-SetupPayload.ps1`. Expected: missing policy script/function and stale exact-version assertions fail.

- [ ] **Step 3: Implement PowerShell policy and Inno checks**

Implement strict numeric parsing with `[version]::TryParse`, require three or four components, and test major/minor. Remove `$versionSupported` based on Streamlabs DisplayVersion. In Inno Setup remove exact Streamlabs comparison, call `GetVersionComponents` on `obs.dll`, and show both detected application/core versions in incompatibility messages.

- [ ] **Step 4: Run policy/setup tests and compile installer source**

Expected: both PowerShell tests exit 0 and Inno Setup compiles without errors.

- [ ] **Step 5: Commit compatibility enforcement**

Commit scripts and installer source with message `Gate installation on OBS Core 31.1.x`.

### Task 3: English-first bilingual public documentation

**Files:**
- Modify: `README.md`
- Create: `README.zh-TW.md`
- Modify: `docs/releases/v0.1.6-beta.md`
- Create: `scripts/Test-Documentation.ps1`

**Interfaces:**
- Both READMEs begin with `[English](README.md) | [繁體中文](README.zh-TW.md)`.
- Documentation test reads both files and validates links, headings, OBS 31.1.x wording, absence of exact app-version gating, and matching checksum values.

- [ ] **Step 1: Write failing documentation contract test**

Require both files, reciprocal links, required mirrored sections, OBS Core 31.1.x wording, and identical 64-character SHA-256 strings. Expected initial failure: `README.zh-TW.md` is missing and `README.md` is Chinese-first.

- [ ] **Step 2: Write complete English and Traditional Chinese documents**

Translate all installation, features, compatibility, recovery, verification, build, license, and trademark sections. Describe Streamlabs 1.21.4 only as the validated example build, not an installer requirement.

- [ ] **Step 3: Update bilingual Release notes**

Change title and compatibility language to OBS Core 31.1.x while retaining the verified-build note and beta warning.

- [ ] **Step 4: Run documentation test and verify GREEN**

Expected: all link, section, compatibility, and checksum checks pass.

- [ ] **Step 5: Commit documentation**

Commit both README files, release notes, and documentation test with message `Add English-first bilingual documentation`.

### Task 4: Rebuild, integration verification, and GitHub Release replacement

**Files:**
- Modify generated ignored artifact: `outputs/MotionFrostedGlass-0.1.6/plugin/motion-frosted-glass.dll`
- Modify generated ignored artifact: `outputs/MotionFrostedGlass-Setup-0.1.6.exe`
- Modify: `README.md`
- Modify: `README.zh-TW.md`
- Modify: `docs/releases/v0.1.6-beta.md`

**Interfaces:**
- Release asset name remains `MotionFrostedGlass-Setup-0.1.6.exe`.
- All three public documents contain the rebuilt installer's exact SHA-256.

- [ ] **Step 1: Build optimized DLL and assemble payload**

Compile against Streamlabs OBS fork tag `31.1.2ndi1`, link against installed Streamlabs `obs.dll` exports, and copy the DLL plus effect/locale assets into the ignored payload directory.

- [ ] **Step 2: Run full native and payload verification**

Run policy, settings, Gaussian, shader contract, D3D11 shader compile, module-load, setup payload, and documentation tests. Run the actual image-filter render integration when the local OBS video subsystem initializes; otherwise report that environment limitation separately without treating partial tests as full integration success.

- [ ] **Step 3: Build installer and update checksum**

Compile with Inno Setup 6, calculate SHA-256, replace the checksum in English README, Chinese README, and Release notes, then rerun documentation and payload tests.

- [ ] **Step 4: Commit generated-metadata updates and push main**

Commit tracked checksum/document changes with message `Publish OBS Core 31.1.x compatible installer`, then push `main`.

- [ ] **Step 5: Replace GitHub prerelease**

Remove and recreate tag/Release `v0.1.6-beta` at the new `main` commit, upload the rebuilt EXE, and use `docs/releases/v0.1.6-beta.md` as notes.

- [ ] **Step 6: Verify remote state**

Use GitHub connector and CLI to confirm public visibility, default branch `main`, tag target equals local HEAD, prerelease status is true, asset size is nonzero, and GitHub asset digest equals the local SHA-256.
