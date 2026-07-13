param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('configure', 'build', 'test')]
    [string]$Action,
    [string]$Target = '',
    [ValidateSet('Debug', 'RelWithDebInfo', 'Release')]
    [string]$Configuration = 'Debug'
)

$ErrorActionPreference = 'Stop'
$env:MSBUILDDISABLENODEREUSE = '1'
$cmake = 'C:\Program Files\CMake\bin\cmake.exe'

switch ($Action) {
    'configure' {
        & $cmake --preset windows-x64 -DBUILD_TESTING=ON
    }
    'build' {
        $arguments = @('--build', 'build_x64', '--config', $Configuration)
        if ($Target) { $arguments += @('--target', $Target) }
        $arguments += @('--', '/nodeReuse:false')
        & $cmake @arguments
    }
    'test' {
        & (Join-Path $PSScriptRoot '..\build_x64\Debug\filter-settings-test.exe')
        if ($LASTEXITCODE -eq 0) {
            & (Join-Path $PSScriptRoot '..\build_x64\Debug\shader-contract-test.exe')
        }
    }
}

exit $LASTEXITCODE
