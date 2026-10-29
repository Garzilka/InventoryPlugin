$ErrorActionPreference = 'Stop'

# Step 2b: editor module includes also use the plugin folder prefix:
#   InventoryEcosystemEditor/06LootSystemEditor/...  ->  InventoryEcosystemEditor/LootSystemEditor/...
# Pure ASCII on purpose (PowerShell 5.1 reads BOM-less files as ANSI).

$src = 'f:\Projects\UnrealWorld\Plugins\InventoryEcosystem\Source'

$mapEd = @(
    @('01_InventoryCoreEditor',  'InventoryCoreEditor'),
    @('03InventorySystemEditor', 'InventorySystemEditor'),
    @('04_HandItemSystemEditor', 'HandItemSystemEditor'),
    @('06LootSystemEditor',      'LootSystemEditor')
)

$utf8Bom   = New-Object System.Text.UTF8Encoding($true)
$utf8NoBom = New-Object System.Text.UTF8Encoding($false)

$changed = 0

Get-ChildItem -Path $src -Recurse -File -Include *.h, *.cpp, *.cs | ForEach-Object {
    $path = $_.FullName
    $bytes = [System.IO.File]::ReadAllBytes($path)
    $hasBom = ($bytes.Length -ge 3 -and $bytes[0] -eq 0xEF -and $bytes[1] -eq 0xBB -and $bytes[2] -eq 0xBF)

    $text = [System.Text.Encoding]::UTF8.GetString($bytes)
    if ($text.Length -gt 0 -and $text[0] -eq [char]0xFEFF) { $text = $text.Substring(1) }
    $original = $text

    foreach ($m in $mapEd) {
        $text = $text.Replace("InventoryEcosystemEditor/$($m[0])/", "InventoryEcosystemEditor/$($m[1])/")
    }

    if ($text -ne $original) {
        $changed++
        $hasNonAscii = ($text -match '[^\x00-\x7F]')
        if ($hasNonAscii -or $hasBom) { [System.IO.File]::WriteAllText($path, $text, $utf8Bom) }
        else                          { [System.IO.File]::WriteAllText($path, $text, $utf8NoBom) }
    }
}

Write-Output ("editor include fixes: {0}" -f $changed)
