$ErrorActionPreference = 'Stop'

# Step 3: extract ItemSystem module out of InventorySystem.
# Pure ASCII on purpose (PowerShell 5.1 reads BOM-less files as ANSI).

$src = 'f:\Projects\UnrealWorld\Plugins\InventoryEcosystem\Source\InventoryEcosystem'
$inv = Join-Path $src 'InventorySystem'
$item = Join-Path $src 'ItemSystem'

$dirs = @('Actor', 'Data', 'Library', 'Objects\Item', 'Objects\ItemServices', 'Objects\MakeItemRule', 'Public', 'Private')
foreach ($d in $dirs) {
    $p = Join-Path $item $d
    if (-not (Test-Path -LiteralPath $p)) { New-Item -ItemType Directory -Path $p -Force | Out-Null }
}

function Move-Pattern([string]$fromDir, [string]$toDir, [string]$pattern) {
    if (-not (Test-Path -LiteralPath $fromDir)) { return }
    Get-ChildItem -LiteralPath $fromDir -File -Filter $pattern | ForEach-Object {
        Move-Item -LiteralPath $_.FullName -Destination (Join-Path $toDir $_.Name) -Force
        Write-Output ("move: {0} -> {1}" -f $_.Name, $toDir)
    }
}

Move-Pattern (Join-Path $inv 'Actor')    (Join-Path $item 'Actor')    'BaseItemActor.*'
Move-Pattern (Join-Path $inv 'Data')     (Join-Path $item 'Data')     'InventoryData.*'
Move-Pattern (Join-Path $inv 'Library')  (Join-Path $item 'Library')  'InventoryLibrary.*'
Move-Pattern (Join-Path $inv 'Objects\Item')        (Join-Path $item 'Objects\Item')        '*.*'
Move-Pattern (Join-Path $inv 'Objects\ItemServices') (Join-Path $item 'Objects\ItemServices') '*.*'
Move-Pattern (Join-Path $inv 'Objects\MakeItemRule') (Join-Path $item 'Objects\MakeItemRule') '*.*'

# settings: InventorySettings -> ItemSettings (class rename is done separately in the file)
$oldSettings = Join-Path $inv 'Public\InventorySettings.h'
if (Test-Path -LiteralPath $oldSettings) {
    Move-Item -LiteralPath $oldSettings -Destination (Join-Path $item 'Public\ItemSettings.h') -Force
    Write-Output 'move: InventorySettings.h -> ItemSystem/Public/ItemSettings.h'
}
$oldSettingsCpp = Join-Path $inv 'Private\InventorySettings.cpp'
if (Test-Path -LiteralPath $oldSettingsCpp) {
    Remove-Item -LiteralPath $oldSettingsCpp -Force
    Write-Output 'remove: empty InventorySettings.cpp'
}

# remove now empty folders
foreach ($d in @('Actor', 'Data', 'Library', 'Objects\Item', 'Objects\ItemServices', 'Objects\MakeItemRule', 'Objects', 'Public', 'Private')) {
    $p = Join-Path $inv $d
    if ((Test-Path -LiteralPath $p) -and -not (Get-ChildItem -LiteralPath $p -Force)) {
        Remove-Item -LiteralPath $p -Force
        Write-Output ("remove empty dir: {0}" -f $d)
    }
}

# ---------------------------------------------------------------- include paths
$repl = @(
    @('InventoryEcosystem/InventorySystem/Objects/Item/',         'InventoryEcosystem/ItemSystem/Objects/Item/'),
    @('InventoryEcosystem/InventorySystem/Objects/ItemServices/', 'InventoryEcosystem/ItemSystem/Objects/ItemServices/'),
    @('InventoryEcosystem/InventorySystem/Objects/MakeItemRule/', 'InventoryEcosystem/ItemSystem/Objects/MakeItemRule/'),
    @('InventoryEcosystem/InventorySystem/Actor/BaseItemActor.h', 'InventoryEcosystem/ItemSystem/Actor/BaseItemActor.h'),
    @('InventoryEcosystem/InventorySystem/Library/InventoryLibrary.h', 'InventoryEcosystem/ItemSystem/Library/InventoryLibrary.h'),
    @('InventoryEcosystem/InventorySystem/Data/InventoryData.h',  'InventoryEcosystem/ItemSystem/Data/InventoryData.h'),
    @('InventoryEcosystem/InventorySystem/Public/InventorySettings.h', 'InventoryEcosystem/ItemSystem/Public/ItemSettings.h')
)

$utf8Bom   = New-Object System.Text.UTF8Encoding($true)
$utf8NoBom = New-Object System.Text.UTF8Encoding($false)

$incChanged = 0
$macroChanged = 0
$sourceRoot = 'f:\Projects\UnrealWorld\Plugins\InventoryEcosystem\Source'

Get-ChildItem -Path $sourceRoot -Recurse -File -Include *.h, *.cpp | ForEach-Object {
    $path = $_.FullName
    $bytes = [System.IO.File]::ReadAllBytes($path)
    $hasBom = ($bytes.Length -ge 3 -and $bytes[0] -eq 0xEF -and $bytes[1] -eq 0xBB -and $bytes[2] -eq 0xBF)

    $text = [System.Text.Encoding]::UTF8.GetString($bytes)
    if ($text.Length -gt 0 -and $text[0] -eq [char]0xFEFF) { $text = $text.Substring(1) }
    $original = $text

    foreach ($r in $repl) { $text = $text.Replace($r[0], $r[1]) }
    if ($text -ne $original) { $incChanged++ }

    if ($path.StartsWith($item, [System.StringComparison]::OrdinalIgnoreCase)) {
        $before = $text
        $text = $text.Replace('INVENTORYSYSTEM_API', 'ITEMSYSTEM_API')
        if ($text -ne $before) { $macroChanged++ }
    }

    if ($text -ne $original) {
        $hasNonAscii = ($text -match '[^\x00-\x7F]')
        if ($hasNonAscii -or $hasBom) { [System.IO.File]::WriteAllText($path, $text, $utf8Bom) }
        else                          { [System.IO.File]::WriteAllText($path, $text, $utf8NoBom) }
    }
}

Write-Output ("files with include changes: {0}" -f $incChanged)
Write-Output ("files with API macro changes: {0}" -f $macroChanged)
