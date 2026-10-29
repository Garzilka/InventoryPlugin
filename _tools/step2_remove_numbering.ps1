$ErrorActionPreference = 'Stop'

# Step 2: drop numeric prefixes from module folders and fix include paths.
# Also: add UTF-8 BOM where file has non-ASCII characters (project rule).
# NOTE: this helper is pure ASCII on purpose (PowerShell 5.1 reads BOM-less files as ANSI).

$src = 'f:\Projects\UnrealWorld\Plugins\InventoryEcosystem\Source'

$map = @(
    @('00_InventoryTools',   'InventoryTools'),
    @('01_InventoryCore',    'InventoryCore'),
    @('02_InteractSystem',   'InteractSystem'),
    @('03_InventorySystem',  'InventorySystem'),
    @('04_HandItemSystem',   'HandItemSystem'),
    @('05_EquipManager',     'EquipManager'),
    @('06_LootSystem',       'LootSystem'),
    @('07_InventoryManager', 'InventoryManagerSystem')
)

$mapEd = @(
    @('01_InventoryCoreEditor',  'InventoryCoreEditor'),
    @('03InventorySystemEditor', 'InventorySystemEditor'),
    @('04_HandItemSystemEditor', 'HandItemSystemEditor'),
    @('06LootSystemEditor',      'LootSystemEditor')
)

$runtimeRoot = Join-Path $src 'InventoryEcosystem'
$editorRoot  = Join-Path $src 'InventoryEcosystemEditor'

foreach ($m in $map) {
    $old = Join-Path $runtimeRoot $m[0]
    if (Test-Path -LiteralPath $old) {
        Rename-Item -LiteralPath $old -NewName $m[1]
        Write-Output ("folder: {0} -> {1}" -f $m[0], $m[1])
    }
}

foreach ($m in $mapEd) {
    $old = Join-Path $editorRoot $m[0]
    if (Test-Path -LiteralPath $old) {
        Rename-Item -LiteralPath $old -NewName $m[1]
        Write-Output ("folder: {0} -> {1}" -f $m[0], $m[1])
    }
}

$utf8Bom   = New-Object System.Text.UTF8Encoding($true)
$utf8NoBom = New-Object System.Text.UTF8Encoding($false)

$allMap = $map + $mapEd
$changed = 0
$bomAdded = 0

Get-ChildItem -Path $src -Recurse -File -Include *.h, *.cpp, *.cs | ForEach-Object {
    $path = $_.FullName
    $bytes = [System.IO.File]::ReadAllBytes($path)
    $hasBom = ($bytes.Length -ge 3 -and $bytes[0] -eq 0xEF -and $bytes[1] -eq 0xBB -and $bytes[2] -eq 0xBF)

    $text = [System.Text.Encoding]::UTF8.GetString($bytes)
    if ($text.Length -gt 0 -and $text[0] -eq [char]0xFEFF) { $text = $text.Substring(1) }
    $original = $text

    foreach ($m in $allMap) {
        $text = $text.Replace("InventoryEcosystem/$($m[0])/", "InventoryEcosystem/$($m[1])/")
    }

    $isChanged   = ($text -ne $original)
    $hasNonAscii = ($text -match '[^\x00-\x7F]')
    $wantBom     = ($hasNonAscii -or $hasBom)
    $needWrite   = ($isChanged -or ($wantBom -and -not $hasBom) -or ((-not $wantBom) -and $hasBom))

    if ($isChanged) { $changed++ }
    if ($wantBom -and (-not $hasBom)) { $bomAdded++ }

    if ($needWrite) {
        if ($wantBom) { [System.IO.File]::WriteAllText($path, $text, $utf8Bom) }
        else          { [System.IO.File]::WriteAllText($path, $text, $utf8NoBom) }
    }
}

Write-Output ("files with include changes: {0}" -f $changed)
Write-Output ("bom added: {0}" -f $bomAdded)
