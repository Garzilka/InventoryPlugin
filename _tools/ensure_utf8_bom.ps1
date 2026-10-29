$ErrorActionPreference = 'Stop'

# Ensure UTF-8 BOM for source files that contain non-ASCII characters (project rule).
# Pure ASCII on purpose (PowerShell 5.1 reads BOM-less files as ANSI).

$src = 'f:\Projects\UnrealWorld\Plugins\InventoryEcosystem\Source'
$utf8Bom = New-Object System.Text.UTF8Encoding($true)
$added = 0

Get-ChildItem -Path $src -Recurse -File -Include *.h, *.cpp, *.cs | ForEach-Object {
    $path = $_.FullName
    $bytes = [System.IO.File]::ReadAllBytes($path)
    $hasBom = ($bytes.Length -ge 3 -and $bytes[0] -eq 0xEF -and $bytes[1] -eq 0xBB -and $bytes[2] -eq 0xBF)

    $text = [System.Text.Encoding]::UTF8.GetString($bytes)
    if ($text.Length -gt 0 -and $text[0] -eq [char]0xFEFF) { $text = $text.Substring(1) }

    $hasNonAscii = ($text -match '[^\x00-\x7F]')
    if ($hasNonAscii -and (-not $hasBom)) {
        [System.IO.File]::WriteAllText($path, $text, $utf8Bom)
        $added++
        Write-Output ("bom+: {0}" -f $_.FullName.Substring($src.Length + 1))
    }
}

Write-Output ("bom added: {0}" -f $added)
