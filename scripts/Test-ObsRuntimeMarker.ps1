$ErrorActionPreference = 'Stop'

. (Join-Path $PSScriptRoot 'ObsVersionPolicy.ps1')

$fixtureRoot = Join-Path ([IO.Path]::GetTempPath()) ('motion-frosted-marker-' + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $fixtureRoot | Out-Null

$fixtures = @(
    @{ Name = 'supported'; Bytes = [Text.Encoding]::ASCII.GetBytes("prefix`031.1.2sl19b3`0suffix"); Expected = '31.1.2sl19b3' },
    @{ Name = 'wrong-fork'; Bytes = [Text.Encoding]::ASCII.GetBytes("prefix`031.1.2ndi1`0suffix"); Expected = '31.1.2ndi1' },
    @{ Name = 'missing'; Bytes = [byte[]](1, 2, 3, 4); Expected = $null }
)

try {
    foreach ($fixture in $fixtures) {
        $path = Join-Path $fixtureRoot ($fixture.Name + '.dll')
        [IO.File]::WriteAllBytes($path, $fixture.Bytes)
        $actual = Get-ObsRuntimeBuildMarker -Path $path
        if ($actual -ne $fixture.Expected) {
            throw "Marker mismatch for $($fixture.Name): expected '$($fixture.Expected)', got '$actual'"
        }
    }

    $supportedPath = Join-Path $fixtureRoot 'supported.dll'
    if (-not (Test-SupportedObsRuntime -Version '31.1.2' -BuildMarker (Get-ObsRuntimeBuildMarker $supportedPath))) {
        throw 'Expected the sl19b3 fixture to pass the combined policy.'
    }

    Write-Output 'All OBS runtime marker tests passed'
} finally {
    Remove-Item -LiteralPath $fixtureRoot -Recurse -Force -ErrorAction SilentlyContinue
}
