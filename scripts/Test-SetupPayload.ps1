$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$package = Join-Path $root 'outputs\MotionFrostedGlass-0.1.6\plugin'
$installerSource = Join-Path $root 'installer\MotionFrostedGlass.iss'
$expectedHash = 'C44DAD94F14932F6D901D60C47542B0859774C1EAC1CF35EDC608FDE51790C20'

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
if ($installerText -notmatch "\(ObsMajor <> 31\) or \(ObsMinor <> 1\)") {
    throw 'Installer does not require OBS Core 31.1.x'
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
