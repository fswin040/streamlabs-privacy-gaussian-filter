$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$package = Join-Path $root 'outputs\MotionFrostedGlass-0.1.6\plugin'
$installerSource = Join-Path $root 'installer\MotionFrostedGlass.iss'
$expectedHash = '56ABF12B3578C1C46996CE8CCAA44E4E32EF000EB8548C59451AD98BA93FE8C7'

if (-not (Test-Path -LiteralPath $installerSource)) { throw "Missing installer source: $installerSource" }
$installerText = Get-Content -LiteralPath $installerSource -Raw
if ($installerText -notmatch "CompareText\(StreamlabsVersion, '1\.21\.4\.0'\)") {
    throw 'Installer does not require Streamlabs Desktop 1.21.4.0'
}
if ($installerText -match "CompareText\(StreamlabsVersion, '1\.21\.3\.0'\)") {
    throw 'Installer still accepts stale Streamlabs Desktop 1.21.3.0'
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
