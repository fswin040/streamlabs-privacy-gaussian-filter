$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'ObsVersionPolicy.ps1')

$registryRoots = @(
    'HKLM:\Software\Microsoft\Windows\CurrentVersion\Uninstall\*',
    'HKLM:\Software\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\*',
    'HKCU:\Software\Microsoft\Windows\CurrentVersion\Uninstall\*'
)
$entry = Get-ItemProperty $registryRoots -ErrorAction SilentlyContinue |
    Where-Object { $_.DisplayName -eq 'Streamlabs Desktop' } |
    Select-Object -First 1

$root = 'C:\Program Files\Streamlabs OBS'
if ($entry.InstallLocation -and (Test-Path -LiteralPath $entry.InstallLocation)) {
    $root = $entry.InstallLocation.TrimEnd('\')
}

$runtime = Join-Path $root 'resources\app.asar.unpacked\node_modules\obs-studio-node'
$obsDll = Join-Path $runtime 'obs.dll'
$pluginRoot = Join-Path $runtime 'obs-plugins\64bit'
$dataRoot = Join-Path $runtime 'data\obs-plugins'
$exe = Join-Path $root 'Streamlabs OBS.exe'
$supported = (Test-Path -LiteralPath $exe) -and (Test-Path -LiteralPath $obsDll) -and
    (Test-Path -LiteralPath $pluginRoot) -and (Test-Path -LiteralPath $dataRoot)
$obsVersion = if (Test-Path -LiteralPath $obsDll) { (Get-Item -LiteralPath $obsDll).VersionInfo.FileVersion } else { $null }
$streamlabsVersion = $entry.DisplayVersion
$versionSupported = Test-SupportedObsVersion -Version $obsVersion

[ordered]@{
    installed = [bool](Test-Path -LiteralPath $exe)
    installRoot = $root
    streamlabsVersion = $streamlabsVersion
    obsVersion = $obsVersion
    architecture = 'x64'
    pluginRoot = $pluginRoot
    dataRoot = $dataRoot
    obsDll = $obsDll
    supported = [bool]($supported -and $versionSupported)
    reason = if (-not $supported) { 'Required Streamlabs OBS runtime paths were not found.' }
             elseif (-not $versionSupported) { "This package requires OBS Core 31.1.x; detected $obsVersion in Streamlabs Desktop $streamlabsVersion." }
             else { 'Compatible.' }
} | ConvertTo-Json -Depth 4

if (-not ($supported -and $versionSupported)) { exit 2 }
