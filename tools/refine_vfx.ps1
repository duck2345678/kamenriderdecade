Add-Type -AssemblyName System.Drawing

$outputDirs = @(
    "C:\Users\ACER\Documents\GitHub\kamenriderdecade\Assets\Textures",
    "C:\Users\ACER\.gemini\antigravity-ide\scratch\kamenriderdecade\Assets\Textures"
)

# 1. Refine Magic Arrow (strictly isolate the flying purple arrow projectile)
$ainzFile = "c:\Users\ACER\Documents\GitHub\kamenriderdecade\69fbd723-d664-4d50-93c0-308b39688126.png"
$ainzBmp = New-Object System.Drawing.Bitmap($ainzFile)
$arrowRect = New-Object System.Drawing.Rectangle(538, 517, 40, 16)
$arrowBmp = $ainzBmp.Clone($arrowRect, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)

foreach ($dir in $outputDirs) {
    $p = Join-Path $dir "vfx_ainz_magic_arrow.png"
    $arrowBmp.Save($p, [System.Drawing.Imaging.ImageFormat]::Png)
    Write-Host "Saved perfect arrow: $p"
}
$arrowBmp.Dispose()
$ainzBmp.Dispose()

# 2. Refine KamenRide Card (Den-O) with full borders
$decadeFile = "C:\Users\ACER\Documents\GitHub\kamenriderdecade\kamen_rider_decade_sprite_sheet_by_neomar654_dgodxu4.png"
$decBmp = New-Object System.Drawing.Bitmap($decadeFile)

$cardRect = New-Object System.Drawing.Rectangle(117, 26342, 137, 186)
$cardBmp = New-Object System.Drawing.Bitmap(137, 186, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
$g = [System.Drawing.Graphics]::FromImage($cardBmp)
$g.DrawImage($decBmp, 0, 0, $cardRect, [System.Drawing.GraphicsUnit]::Pixel)
$g.Dispose()

# Remove background green around card
for ($y = 0; $y -lt $cardBmp.Height; $y++) {
    for ($x = 0; $x -lt $cardBmp.Width; $x++) {
        $c = $cardBmp.GetPixel($x, $y)
        if ($c.R -le 25 -and $c.G -ge 85 -and $c.G -le 115 -and $c.B -le 25) {
            $cardBmp.SetPixel($x, $y, [System.Drawing.Color]::FromArgb(0, 0, 0, 0))
        }
    }
}

foreach ($dir in $outputDirs) {
    $p = Join-Path $dir "vfx_decade_card.png"
    $cardBmp.Save($p, [System.Drawing.Imaging.ImageFormat]::Png)
    Write-Host "Saved perfect card: $p"
}

$cardBmp.Dispose()
$decBmp.Dispose()

Write-Host "VFX perfection completed!"
