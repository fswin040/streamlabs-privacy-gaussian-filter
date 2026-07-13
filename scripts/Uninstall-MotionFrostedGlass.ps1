$ErrorActionPreference = 'Stop'
$detection = & (Join-Path $PSScriptRoot 'Detect-Streamlabs.ps1') | ConvertFrom-Json
$targetDll = Join-Path $detection.pluginRoot 'motion-frosted-glass.dll'
$targetData = Join-Path $detection.dataRoot 'motion-frosted-glass'

if (Get-Process -Name 'Streamlabs OBS' -ErrorAction SilentlyContinue) {
    throw 'Please close Streamlabs Desktop before uninstalling the plugin.'
}

if (Test-Path -LiteralPath $targetDll) { Remove-Item -LiteralPath $targetDll -Force }
if (Test-Path -LiteralPath $targetData) {
    $resolvedData = (Resolve-Path -LiteralPath $targetData).Path
    $resolvedRoot = (Resolve-Path -LiteralPath $detection.dataRoot).Path
    if (-not $resolvedData.StartsWith($resolvedRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw 'Refusing to remove a directory outside the Streamlabs plugin data root.'
    }
    Remove-Item -LiteralPath $resolvedData -Recurse -Force
}
Write-Host 'Motion Frosted Glass removed successfully.'

