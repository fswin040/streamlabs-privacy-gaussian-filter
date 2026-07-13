$ErrorActionPreference = 'Stop'

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
$versionSupported = ($streamlabsVersion -eq '1.21.4') -or ($streamlabsVersion -eq '1.21.4.0')

[ordered]@{
    installed = [bool](Test-Path -LiteralPath $exe)
    installRoot = $root
    streamlabsVersion = $streamlabsVersion
    obsVersion = $obsVersion
    architecture = 'x64'
    pluginRoot = $pluginRoot
    dataRoot = $dataRoot
    obsDll = $obsDll
    supported = [bool]($supported -and $versionSupported -and $obsVersion -eq '31.1.2')
    reason = if (-not $supported) { 'Required Streamlabs OBS runtime paths were not found.' }
             elseif (-not $versionSupported) { "This package targets Streamlabs Desktop 1.21.4; detected $streamlabsVersion." }
             elseif ($obsVersion -ne '31.1.2') { "This package targets OBS 31.1.2; detected $obsVersion." }
             else { 'Compatible.' }
} | ConvertTo-Json -Depth 4

if (-not ($supported -and $versionSupported -and $obsVersion -eq '31.1.2')) { exit 2 }
