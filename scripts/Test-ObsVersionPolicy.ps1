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

if (-not (Test-SupportedObsRuntime -Version '31.1.2' -BuildMarker '31.1.2sl19b3')) {
    throw 'Expected verified sl19b3 runtime to be supported.'
}
foreach ($marker in @('31.1.2ndi1', '31.1.2sl19b2', '31.1.2sl22', '')) {
    if (Test-SupportedObsRuntime -Version '31.1.2' -BuildMarker $marker) {
        throw "Expected unverified runtime marker to be rejected: $marker"
    }
}
