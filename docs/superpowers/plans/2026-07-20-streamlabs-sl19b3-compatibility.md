# Streamlabs `31.1.2sl19b3` Compatibility Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Restore Pure Gaussian Blur rendering on Streamlabs Desktop 1.21.7 and ship a fail-closed v0.1.7-beta installer for the verified `31.1.2sl19b3` native ABI.

**Architecture:** Compile the plugin with the corrected append-only `obs_source_info` layout from the Streamlabs `31.1.2sl19b3` fork, then guard both module registration and installation with the exact runtime build marker. Preserve the existing `shader_filter` ID and settings schema so users' scene collections continue to work without migration.

**Tech Stack:** C++17, libobs/Direct3D 11 effect files, CMake/Visual Studio 2022, PowerShell, Inno Setup 6, GitHub Releases.

## Global Constraints

- Target Windows x64 Streamlabs Desktop 1.21.7.
- Accept runtime numeric API 31.1.x only when the runtime build string is exactly `31.1.2sl19b3`.
- Use the corrected `31.1.2sl19b3` `obs_source_info` member order.
- Keep source ID `shader_filter` and all existing scene setting names unchanged.
- Do not modify users' scene collections.
- Fail closed before file replacement or module registration on an unknown runtime.
- Produce `MotionFrostedGlass-Setup-0.1.7.exe` with English and Traditional Chinese documentation.

---

### Task 1: Make the current rendering failure reproducible from the build

**Files:**
- Modify: `CMakeLists.txt`
- Test: `tests/image-filter-render-test.cpp`

**Interfaces:**
- Consumes: the installed Streamlabs `obs.dll`, `libobs-d3d11.dll`, `image-source.dll`, and plugin data directories.
- Produces: CMake target `image-filter-render-test` and an integration command whose exit code is zero only when real filter callbacks and Gaussian passes execute.

- [ ] **Step 1: Register the existing image integration test as a Windows build target**

Add inside `if(BUILD_TESTING)`:

```cmake
if(OS_WINDOWS)
  add_executable(image-filter-render-test tests/image-filter-render-test.cpp)
  target_link_libraries(image-filter-render-test PRIVATE OBS::libobs)
  target_compile_features(image-filter-render-test PRIVATE cxx_std_17)
  target_include_directories(image-filter-render-test BEFORE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/compat/libobs")
endif()
```

- [ ] **Step 2: Configure and build the regression test without changing the ABI header**

Run:

```powershell
& scripts/Invoke-CleanCMake.ps1 -Action configure
& scripts/Invoke-CleanCMake.ps1 -Action build -Target image-filter-render-test -Configuration RelWithDebInfo
```

Expected: build succeeds.

- [ ] **Step 3: Run against Streamlabs 1.21.7 and verify the red state**

Run:

```powershell
$runtime = 'C:\Program Files\Streamlabs OBS\resources\app.asar.unpacked\node_modules\obs-studio-node'
$env:PATH = "$runtime;$runtime\obs-plugins\64bit;$env:PATH"
$arguments = @(
  "$runtime\libobs-d3d11.dll",
  "$runtime\data\libobs",
  "$runtime\obs-plugins\64bit\image-source.dll",
  "$runtime\data\obs-plugins\image-source",
  "$runtime\obs-plugins\64bit\motion-frosted-glass.dll",
  "$runtime\data\obs-plugins\motion-frosted-glass",
  'C:\Users\71133\AppData\Local\Temp\codex-clipboard-d7a1e4a0-9f18-44c9-8c69-8e0a9fbc3b6f.png'
)
& build_x64/RelWithDebInfo/image-filter-render-test.exe @arguments
```

Expected: exit code 8 with `callbacks=0`, `gaussian_passes=0`, and render contract errors. This is the required failing regression before production changes.

- [ ] **Step 4: Commit the reproducible test target**

```powershell
git add CMakeLists.txt
git commit -m "test: reproduce Streamlabs sl19b3 render failure"
```

### Task 2: Correct the native ABI and runtime registration policy

**Files:**
- Modify: `compat/libobs/obs-source.h`
- Modify: `src/obs-version-policy.hpp`
- Modify: `src/obs-version-policy.cpp`
- Modify: `src/plugin-main.cpp`
- Modify: `tests/obs-version-policy-test.cpp`

**Interfaces:**
- Consumes: `obs_get_version()` and `obs_get_version_string()`.
- Produces: `bool motion_frosted_obs_runtime_supported(uint32_t version, const char *build_string) noexcept`.

- [ ] **Step 1: Write the failing exact-build policy test**

Replace broad 31.1.x assertions with:

```cpp
if (!motion_frosted_obs_runtime_supported(make_version(31, 1, 3), "31.1.2sl19b3"))
    return 1;
if (motion_frosted_obs_runtime_supported(make_version(31, 1, 3), "31.1.2ndi1"))
    return 1;
if (motion_frosted_obs_runtime_supported(make_version(31, 1, 3), "31.1.2sl19b4"))
    return 1;
if (motion_frosted_obs_runtime_supported(make_version(31, 2, 0), "31.1.2sl19b3"))
    return 1;
if (motion_frosted_obs_runtime_supported(make_version(31, 1, 3), nullptr))
    return 1;
```

- [ ] **Step 2: Build the policy test and verify it fails**

Run:

```powershell
& scripts/Invoke-CleanCMake.ps1 -Action build -Target obs-version-policy-test -Configuration RelWithDebInfo
```

Expected: compile failure because `motion_frosted_obs_runtime_supported` does not exist.

- [ ] **Step 3: Implement the minimal exact-build policy**

Declare and implement:

```cpp
bool motion_frosted_obs_runtime_supported(uint32_t version, const char *build_string) noexcept
{
    const uint32_t major = version >> 24U;
    const uint32_t minor = (version >> 16U) & 0xFFU;
    return major == 31U && minor == 1U && build_string &&
           std::strcmp(build_string, "31.1.2sl19b3") == 0;
}
```

Update `obs_module_load()` to pass both runtime values and to log that the build requires `31.1.2sl19b3` when rejected.

- [ ] **Step 4: Correct the compatibility structure layout**

In `compat/libobs/obs-source.h`, remove these members from immediately after `activate`:

```cpp
void (*message)(void *data, obs_data_t *settings);
obs_data_array_t *(*get_messages)(void *data);
```

Append the same members after `filter_add`, matching Streamlabs fork commit `7fef2cce49a6e8c748e7c2da94ef3ee0dec69013`.

- [ ] **Step 5: Build and run unit and shader tests**

Run:

```powershell
& scripts/Invoke-CleanCMake.ps1 -Action build -Configuration RelWithDebInfo
& build_x64/RelWithDebInfo/obs-version-policy-test.exe
& build_x64/RelWithDebInfo/filter-settings-test.exe
& build_x64/RelWithDebInfo/shader-contract-test.exe
```

Expected: all tests pass.

- [ ] **Step 6: Run the current-runtime image integration test with the rebuilt DLL**

Run:

```powershell
$runtime = 'C:\Program Files\Streamlabs OBS\resources\app.asar.unpacked\node_modules\obs-studio-node'
$env:PATH = "$runtime;$runtime\obs-plugins\64bit;$env:PATH"
$arguments = @(
  "$runtime\libobs-d3d11.dll",
  "$runtime\data\libobs",
  "$runtime\obs-plugins\64bit\image-source.dll",
  "$runtime\data\obs-plugins\image-source",
  'build_x64\RelWithDebInfo\motion-frosted-glass.dll',
  'data',
  'C:\Users\71133\AppData\Local\Temp\codex-clipboard-d7a1e4a0-9f18-44c9-8c69-8e0a9fbc3b6f.png'
)
& build_x64/RelWithDebInfo/image-filter-render-test.exe @arguments
```

Expected: exit code 0, renderer callbacks above 300, Gaussian passes at least four times the callback count, and zero render errors.

- [ ] **Step 7: Commit the ABI fix**

```powershell
git add compat/libobs/obs-source.h src/obs-version-policy.hpp src/obs-version-policy.cpp src/plugin-main.cpp tests/obs-version-policy-test.cpp
git commit -m "fix: target Streamlabs sl19b3 filter ABI"
```

### Task 3: Make installer and detection fail closed on unknown ABI builds

**Files:**
- Modify: `scripts/ObsVersionPolicy.ps1`
- Modify: `scripts/Test-ObsVersionPolicy.ps1`
- Modify: `scripts/Detect-Streamlabs.ps1`
- Modify: `scripts/Install-MotionFrostedGlass.ps1`
- Create: `scripts/Test-ObsRuntimeMarker.ps1`
- Modify: `installer/MotionFrostedGlass.iss`
- Modify: `scripts/Test-SetupPayload.ps1`

**Interfaces:**
- Produces: `Get-ObsRuntimeBuildMarker -Path <string>` returning the detected marker or `$null`.
- Produces: `Test-SupportedObsRuntime -Version <string> -BuildMarker <string>` returning a Boolean.

- [ ] **Step 1: Write failing PowerShell marker and installer source tests**

Create temporary binary fixtures containing `31.1.2sl19b3`, `31.1.2sl19b4`, and no marker. Assert that only the first is accepted with numeric version `31.1.2`.

Update `Test-SetupPayload.ps1` to require the literal verified marker and reject the old major/minor-only installer rule.

- [ ] **Step 2: Run the PowerShell tests and verify red**

```powershell
& scripts/Test-ObsVersionPolicy.ps1
& scripts/Test-ObsRuntimeMarker.ps1
& scripts/Test-SetupPayload.ps1
```

Expected: failures because marker functions and v0.1.7 payload do not exist.

- [ ] **Step 3: Implement marker extraction and policy**

Use binary-safe ASCII scanning:

```powershell
function Get-ObsRuntimeBuildMarker {
    param([Parameter(Mandatory)][string] $Path)
    if (-not (Test-Path -LiteralPath $Path)) { return $null }
    $text = [Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($Path))
    $match = [regex]::Match($text, '31\.1\.2(?:sl|ndi)[A-Za-z0-9.-]+')
    if ($match.Success) { return $match.Value }
    return $null
}

function Test-SupportedObsRuntime {
    param([string] $Version, [string] $BuildMarker)
    return (Test-SupportedObsVersion -Version $Version) -and
           $BuildMarker -eq '31.1.2sl19b3'
}
```

Return `obsBuildMarker` in detection JSON and require the combined policy before installation.

- [ ] **Step 4: Add equivalent fail-closed Inno Setup detection**

Add binary marker extraction directly in Pascal Script:

```pascal
function IsBuildMarkerChar(Value: AnsiChar): Boolean;
begin
  Result := ((Value >= '0') and (Value <= '9')) or
            ((Value >= 'A') and (Value <= 'Z')) or
            ((Value >= 'a') and (Value <= 'z')) or
            (Value = '.') or (Value = '-');
end;

function GetObsBuildMarker: String;
var
  Data: AnsiString;
  StartIndex: Integer;
  EndIndex: Integer;
  Candidate: AnsiString;
begin
  Result := 'unknown';
  if not LoadStringFromFile(GetRuntimeRoot + '\obs.dll', Data) then
    Exit;

  for StartIndex := 1 to Length(Data) - 6 do
  begin
    if Copy(Data, StartIndex, 7) = '31.1.2' then
    begin
      EndIndex := StartIndex + 7;
      while (EndIndex <= Length(Data)) and
            ((EndIndex - StartIndex) < 64) and
            IsBuildMarkerChar(Data[EndIndex]) do
        EndIndex := EndIndex + 1;
      Candidate := Copy(Data, StartIndex, EndIndex - StartIndex);
      if (Pos('31.1.2sl', Candidate) = 1) or
         (Pos('31.1.2ndi', Candidate) = 1) then
      begin
        Result := String(Candidate);
        Exit;
      end;
    end;
  end;
end;
```

Call `GetObsBuildMarker` in `InitializeSetup`, require `CompareText(ObsBuildMarker, '31.1.2sl19b3') = 0`, and show both detected numeric version and build marker in the bilingual error. Perform this check before `StreamlabsIsRunning` and before `[Files]` replacement begins.

- [ ] **Step 5: Run the PowerShell contract tests**

Expected: version, marker, and installer source tests pass.

- [ ] **Step 6: Commit installer safety**

```powershell
git add scripts installer/MotionFrostedGlass.iss
git commit -m "fix: reject unverified Streamlabs native ABIs"
```

### Task 4: Package v0.1.7 and update bilingual release documentation

**Files:**
- Modify: `buildspec.json`
- Modify: `README.md`
- Modify: `README.zh-TW.md`
- Modify: `scripts/Test-Documentation.ps1`
- Create: `docs/releases/v0.1.7-beta.md`
- Create/update: `outputs/MotionFrostedGlass-0.1.7/plugin/**`

**Interfaces:**
- Produces: versioned plugin payload and bilingual documentation referencing one installer digest.

- [ ] **Step 1: Write failing documentation and package-version assertions**

Require both READMEs to contain `MotionFrostedGlass-Setup-0.1.7.exe`, `31.1.2sl19b3`, Streamlabs Desktop 1.21.7, and identical SHA-256 values. Reject remaining claims that every 31.1.x build is compatible.

- [ ] **Step 2: Run the documentation test and verify red**

Expected: failure while the files still describe v0.1.6 and broad 31.1.x compatibility.

- [ ] **Step 3: Increment versions and stage the plugin payload**

Set `buildspec.json`, the Inno definitions, PowerShell manifest, and payload paths to `0.1.7`. Copy the rebuilt DLL plus unchanged effects/locales into `outputs/MotionFrostedGlass-0.1.7/plugin`.

- [ ] **Step 4: Build the installer**

```powershell
& "$env:LOCALAPPDATA\Programs\Inno Setup 6\ISCC.exe" installer/MotionFrostedGlass.iss
```

Expected: `outputs/MotionFrostedGlass-Setup-0.1.7.exe` exists.

- [ ] **Step 5: Calculate the installer digest and update all documentation**

Write the uppercase SHA-256 into both READMEs and `docs/releases/v0.1.7-beta.md`. Describe v0.1.6's broad 31.1.x claim as superseded.

- [ ] **Step 6: Run package and documentation tests**

Expected: all payload paths, version strings, ABI markers, and checksums match.

- [ ] **Step 7: Commit the release package source and documentation**

```powershell
git add buildspec.json installer scripts README.md README.zh-TW.md docs/releases/v0.1.7-beta.md
git commit -m "release: prepare v0.1.7-beta for Streamlabs 1.21.7"
```

### Task 5: Install, verify, and publish the release

**Files:**
- Verify: `outputs/MotionFrostedGlass-Setup-0.1.7.exe`
- Verify: installed Streamlabs plugin and data files

**Interfaces:**
- Consumes: the completed installer and local Streamlabs 1.21.7 runtime.
- Produces: a verified local installation and GitHub `v0.1.7-beta` release asset.

- [ ] **Step 1: Confirm Streamlabs is closed and run the installer as administrator**

Expected: installation succeeds without touching scene collection JSON.

- [ ] **Step 2: Verify installed hashes and module initialization**

Compare installed DLL/effect hashes with the payload, then run module-load and shader-runtime tests.

- [ ] **Step 3: Run the image-filter integration test against installed files**

Expected: non-zero callbacks and Gaussian passes, zero render errors, exit code 0.

- [ ] **Step 4: Run the full verification suite**

Run CTest/unit tests, PowerShell policy tests, setup payload test, documentation test, shader runtime test, and `git diff --check`.

Expected: every command exits zero and `git status --short` contains only intentionally ignored release binaries.

- [ ] **Step 5: Push the commits and publish v0.1.7-beta**

Push `main`, create the GitHub release using `docs/releases/v0.1.7-beta.md`, upload `MotionFrostedGlass-Setup-0.1.7.exe`, and verify the remote asset digest matches the documented SHA-256.

- [ ] **Step 6: Report the installed and published result**

Provide the release URL, local EXE path, SHA-256, verified Streamlabs/OBS build, and confirmation that the user's existing filter settings were preserved.
