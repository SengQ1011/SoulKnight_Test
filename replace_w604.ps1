# Replace w604 with w600 in ObjectPosition JSON files
$files = @(
    "json\IcePlains\BossRoom\BossObjectPosition_1.json",
    "json\IcePlains\MonsterRoom\1717ObjectPosition_1.json",
    "json\IcePlains\MonsterRoom\2323ObjectPosition_1.json", 
    "json\IcePlains\MonsterRoom\2717ObjectPosition_1.json",
    "json\IcePlains\MonsterRoom\ObjectPosition.json",
    "json\IcePlains\PortalRoom\ObjectPosition.json",
    "json\IcePlains\SpecialRoom\ShopObjectPosition.json",
    "json\IcePlains\StartingRoom\ObjectPosition.json"
)

foreach ($file in $files) {
    if (Test-Path $file) {
        Write-Host "Processing $file ..."
        $content = Get-Content $file -Raw
        $newContent = $content -replace '"w604"', '"w600"'
        Set-Content $file $newContent -NoNewline
        Write-Host "Completed: $file"
    }
    else {
        Write-Host "File not found: $file"
    }
}

Write-Host "All files processed!" 