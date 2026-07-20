$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $PSScriptRoot
$englishPath = Join-Path $root 'README.md'
$chinesePath = Join-Path $root 'README.zh-TW.md'
$selector = '[English](README.md) | [繁體中文](README.zh-TW.md)'

if (-not (Test-Path -LiteralPath $englishPath)) { throw 'Missing English README.md' }
if (-not (Test-Path -LiteralPath $chinesePath)) { throw 'Missing Traditional Chinese README.zh-TW.md' }

$english = Get-Content -LiteralPath $englishPath -Raw
$chinese = Get-Content -LiteralPath $chinesePath -Raw

if (-not $english.StartsWith($selector)) { throw 'English README does not begin with the language selector' }
if (-not $chinese.StartsWith($selector)) { throw 'Chinese README does not begin with the language selector' }
if ($english -notmatch '# Streamlabs Privacy Gaussian Filter') { throw 'English README is not the default English landing page' }
if ($chinese -notmatch '# Streamlabs 隱私高斯模糊濾鏡') { throw 'Chinese README title is missing' }

$englishSections = @('Download and install', 'Features', 'Compatibility and safety', 'Remove or recover', 'Verify the download', 'Build from source', 'License and trademarks')
foreach ($section in $englishSections) {
    if ($english -notmatch [regex]::Escape("## $section")) { throw "English README missing section: $section" }
}

$chineseSections = @('下載與安裝', '功能', '相容性與安全性', '移除或復原', '驗證下載檔案', '從原始碼建置', '授權與商標')
foreach ($section in $chineseSections) {
    if ($chinese -notmatch [regex]::Escape("## $section")) { throw "Chinese README missing section: $section" }
}

foreach ($document in @($english, $chinese)) {
    if ($document -notmatch '31\.1\.2sl19b3') { throw 'README does not state the exact verified ABI marker' }
    if ($document -notmatch 'Streamlabs Desktop 1\.21\.7') { throw 'README does not state the validated Streamlabs version' }
    if ($document -notmatch 'MotionFrostedGlass-Setup-0\.1\.8\.exe') { throw 'README does not reference the v0.1.8 installer' }
    if ($document -match 'supports Streamlabs Desktop builds whose embedded \*\*OBS Core API is 31\.1\.x|支援內建 \*\*OBS Core API 31\.1\.x') {
        throw 'README still makes a broad 31.1.x ABI compatibility claim'
    }
    if ($document -match 'only supports Streamlabs Desktop 1\.21\.4|只支援 Streamlabs Desktop 1\.21\.4') {
        throw 'README still claims exact Streamlabs Desktop 1.21.4 gating'
    }
}

$hashPattern = '(?im)^SHA-256\s+([A-F0-9]{64})$'
$englishHash = [regex]::Match($english, $hashPattern).Groups[1].Value
$chineseHash = [regex]::Match($chinese, $hashPattern).Groups[1].Value
if (-not $englishHash -or -not $chineseHash) { throw 'README checksum is missing' }
if ($englishHash -ne $chineseHash) { throw 'English and Chinese README checksums differ' }
if ($englishHash -ne '3BB6F49151289015D37C867EADCB928FC1EEFEF1CBC8FEB32D4630BBE6B7D2D7') {
    throw 'README checksum does not match the v0.1.8 installer'
}

Write-Output "Documentation contract valid: $englishHash"
