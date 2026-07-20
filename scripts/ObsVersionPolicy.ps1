function Test-SupportedObsVersion {
    [CmdletBinding()]
    param(
        [AllowEmptyString()]
        [string] $Version
    )

    if ([string]::IsNullOrWhiteSpace($Version)) { return $false }

    $parsed = $null
    if (-not [version]::TryParse($Version, [ref] $parsed)) { return $false }

    return $parsed.Major -eq 31 -and $parsed.Minor -eq 1 -and $parsed.Build -ge 0
}

function Get-ObsRuntimeBuildMarker {
    [CmdletBinding()]
    param([Parameter(Mandatory)][string] $Path)

    if (-not (Test-Path -LiteralPath $Path)) { return $null }
    $text = [Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($Path))
    $match = [regex]::Match($text, '31\.1\.2(?:sl|ndi)[A-Za-z0-9.-]+')
    if ($match.Success) { return $match.Value }
    return $null
}

function Test-SupportedObsRuntime {
    [CmdletBinding()]
    param([string] $Version, [string] $BuildMarker)

    return (Test-SupportedObsVersion -Version $Version) -and
        $BuildMarker -eq '31.1.2sl19b3'
}
