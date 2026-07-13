$ErrorActionPreference = 'Stop'

. (Join-Path $PSScriptRoot 'ObsVersionPolicy.ps1')

$accepted = @('31.1.0', '31.1.2.0', '31.1.3', '31.1.99.7')
foreach ($version in $accepted) {
    if (-not (Test-SupportedObsVersion -Version $version)) {
        throw "Expected supported OBS version: $version"
    }
}

$rejected = @('31.0.99', '31.2.0', '32.0.0', '', 'not-a-version', '31.1')
foreach ($version in $rejected) {
    if (Test-SupportedObsVersion -Version $version) {
        throw "Expected unsupported OBS version: $version"
    }
}

Write-Output 'All PowerShell OBS version policy tests passed'
