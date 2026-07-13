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
