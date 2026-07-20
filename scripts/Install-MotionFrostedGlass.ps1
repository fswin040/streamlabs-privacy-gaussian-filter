param(
    [string]$StreamlabsRoot = '',
    [switch]$WhatIf
)

$ErrorActionPreference = 'Stop'
$isAdministrator = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole(
    [Security.Principal.WindowsBuiltInRole]::Administrator
)
if (-not $WhatIf -and -not $isAdministrator) {
    $arguments = @('-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', "`"$PSCommandPath`"")
    if ($StreamlabsRoot) { $arguments += @('-StreamlabsRoot', "`"$StreamlabsRoot`"") }
    $elevated = Start-Process -FilePath 'pwsh.exe' -ArgumentList $arguments -Verb RunAs -Wait -PassThru
    exit $elevated.ExitCode
}

$packageRoot = Split-Path -Parent $PSScriptRoot
$detection = & (Join-Path $PSScriptRoot 'Detect-Streamlabs.ps1') | ConvertFrom-Json
if (-not $detection.supported) { throw $detection.reason }
if ($StreamlabsRoot) {
    $runtime = Join-Path $StreamlabsRoot 'resources\app.asar.unpacked\node_modules\obs-studio-node'
    $detection.pluginRoot = Join-Path $runtime 'obs-plugins\64bit'
    $detection.dataRoot = Join-Path $runtime 'data\obs-plugins'
}

$sourceDll = Join-Path $packageRoot 'plugin\motion-frosted-glass.dll'
$sourceData = Join-Path $packageRoot 'plugin\data'
$targetDll = Join-Path $detection.pluginRoot 'motion-frosted-glass.dll'
$targetData = Join-Path $detection.dataRoot 'motion-frosted-glass'
if (-not (Test-Path -LiteralPath $sourceDll)) { throw "Missing package DLL: $sourceDll" }
if (-not (Test-Path -LiteralPath $sourceData)) { throw "Missing package data: $sourceData" }

$files = @($targetDll)
$files += Get-ChildItem -LiteralPath $sourceData -Recurse -File | ForEach-Object {
    Join-Path $targetData $_.FullName.Substring($sourceData.Length).TrimStart('\')
}

if ($WhatIf) {
    [ordered]@{ targetDll = $targetDll; targetData = $targetData; files = $files } | ConvertTo-Json -Depth 4
    exit 0
}

if (Get-Process -Name 'Streamlabs OBS' -ErrorAction SilentlyContinue) {
    throw 'Please close Streamlabs Desktop before installing the plugin.'
}

New-Item -ItemType Directory -Force -Path $detection.pluginRoot, $targetData | Out-Null
Copy-Item -LiteralPath $sourceDll -Destination $targetDll -Force
Get-ChildItem -LiteralPath $sourceData -Force | Copy-Item -Destination $targetData -Recurse -Force

$manifest = [ordered]@{
    pluginVersion = '0.1.8'
    targetObsVersion = $detection.obsVersion
    targetObsBuildMarker = $detection.obsBuildMarker
    installedAt = (Get-Date).ToString('o')
    files = $files
}
$manifest | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $targetData 'install-manifest.json') -Encoding UTF8
Write-Host 'Motion Frosted Glass installed successfully.'
