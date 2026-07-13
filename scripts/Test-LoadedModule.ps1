param(
    [Parameter(Mandatory = $true)]
    [int]$ProcessId,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath
)

$ErrorActionPreference = 'Stop'
$modules = Get-Process -Id $ProcessId -Module | Where-Object {
    $_.ModuleName -match 'motion-frosted-glass|obs.dll|libobs-d3d11'
} | Select-Object ModuleName, FileName
$modules | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath $OutputPath -Encoding UTF8
