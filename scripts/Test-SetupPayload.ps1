$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$package = Join-Path $root 'outputs\MotionFrostedGlass-0.1.7\plugin'
$installerSource = Join-Path $root 'installer\MotionFrostedGlass.iss'
$expectedHash = 'A304D205570F2F148647B813B600B92F8B451D221EC705B64ABC65632153F106'

if (-not (Test-Path -LiteralPath $installerSource)) { throw "Missing installer source: $installerSource" }
$installerText = Get-Content -LiteralPath $installerSource -Raw
if ($installerText -match "CompareText\(StreamlabsVersion") {
    throw 'Installer still gates on an exact Streamlabs Desktop version'
}
if ($installerText -match "CompareText\(ObsVersion") {
    throw 'Installer still gates on an exact OBS patch version'
}
if ($installerText -notmatch "GetVersionComponents\(GetRuntimeRoot \+ '\\obs\.dll', ObsMajor, ObsMinor, ObsRevision, ObsBuild\)") {
    throw 'Installer does not decode numeric obs.dll version components'
}
if ($installerText -notmatch "31\.1\.2sl19b3" -or
    $installerText -notmatch "CompareText\(ObsBuildMarker, '31\.1\.2sl19b3'\)") {
    throw 'Installer does not fail closed on the verified sl19b3 ABI marker'
}

$required = @(
    'motion-frosted-glass.dll',
    'data\effects\blur.effect',
    'data\effects\frosted-glass.effect',
    'data\effects\output.effect',
    'data\locale\en-US.ini',
    'data\locale\zh-TW.ini'
)
foreach ($relative in $required) {
    $path = Join-Path $package $relative
    if (-not (Test-Path -LiteralPath $path)) { throw "Missing setup payload: $relative" }
}

$actualHash = (Get-FileHash -Algorithm SHA256 (Join-Path $package 'motion-frosted-glass.dll')).Hash
if ($actualHash -ne $expectedHash) { throw "Unexpected DLL hash: $actualHash" }

Write-Output "Setup payload valid: $actualHash"
