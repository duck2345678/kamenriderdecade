Add-Type -AssemblyName System.Drawing

$outputDirs = @(
    "C:\Users\ACER\Documents\GitHub\kamenriderdecade\Assets\Textures",
    "C:\Users\ACER\.gemini\antigravity-ide\scratch\kamenriderdecade\Assets\Textures"
)

# 1. Extract Magic Arrow Projectile from Ainz sheet (69fbd723-d664-4d50-93c0-308b39688126.png)
$ainzFile = "c:\Users\ACER\Documents\GitHub\kamenriderdecade\69fbd723-d664-4d50-93c0-308b39688126.png"
$ainzBmp = New-Object System.Drawing.Bitmap($ainzFile)

# The middle horizontal magic arrow at frame 4 of Magic Arrow
# Location in full sheet: X=530..575, Y=518..535
$arrowRect = New-Object System.Drawing.Rectangle(525, 515, 55, 24)
$arrowBmp = $ainzBmp.Clone($arrowRect, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)

foreach ($dir in $outputDirs) {
    $p = Join-Path $dir "vfx_ainz_magic_arrow.png"
    $arrowBmp.Save($p, [System.Drawing.Imaging.ImageFormat]::Png)
    Write-Host "Saved: $p"
}
$arrowBmp.Dispose()
$ainzBmp.Dispose()

# 2. Extract authentic Rider Card from Decade sheet
$decadeFile = "C:\Users\ACER\Documents\GitHub\kamenriderdecade\kamen_rider_decade_sprite_sheet_by_neomar654_dgodxu4.png"
$decBmp = New-Object System.Drawing.Bitmap($decadeFile)

# From our scan, Den-O / Decade card is located around X=100, Y=26575, Width=130, Height=185
$cardRect = New-Object System.Drawing.Rectangle(100, 26572, 130, 186)
$cardBmp = New-Object System.Drawing.Bitmap(130, 186, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
$g = [System.Drawing.Graphics]::FromImage($cardBmp)
$g.DrawImage($decBmp, 0, 0, $cardRect, [System.Drawing.GraphicsUnit]::Pixel)
$g.Dispose()

# Remove green background on the card outer border if any
for ($y = 0; $y -lt $cardBmp.Height; $y++) {
    for ($x = 0; $x -lt $cardBmp.Width; $x++) {
        $c = $cardBmp.GetPixel($x, $y)
        if ($c.R -le 10 -and $c.G -ge 95 -and $c.G -le 110 -and $c.B -le 10) {
            $cardBmp.SetPixel($x, $y, [System.Drawing.Color]::FromArgb(0, 0, 0, 0))
        }
    }
}

foreach ($dir in $outputDirs) {
    $p = Join-Path $dir "vfx_decade_card.png"
    $cardBmp.Save($p, [System.Drawing.Imaging.ImageFormat]::Png)
    Write-Host "Saved: $p"
}
$cardBmp.Dispose()
$decBmp.Dispose()

# 3. Create Decade's Ride Booker Magenta Laser Bullet (vfx_bullet_decade.png)
# High precision pixel art laser bullet (32 x 10 px) with glowing magenta/cyan aura
$bulletBmp = New-Object System.Drawing.Bitmap(32, 10, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
for ($y = 0; $y -lt 10; $y++) {
    for ($x = 0; $x -lt 32; $x++) {
        $distY = [Math]::Abs($y - 4.5)
        if ($distY -le 1.0 -and $x -ge 4) {
            # Bright white core
            $bulletBmp.SetPixel($x, $y, [System.Drawing.Color]::FromArgb(255, 255, 255, 255))
        } elseif ($distY -le 2.5 -and $x -ge 2) {
            # Intense Magenta (Decade's signature color)
            $bulletBmp.SetPixel($x, $y, [System.Drawing.Color]::FromArgb(230, 255, 0, 128))
        } elseif ($distY -le 4.5) {
            # Outer glowing aura
            $alpha = [int](140 * (1.0 - ($distY / 4.5)))
            $bulletBmp.SetPixel($x, $y, [System.Drawing.Color]::FromArgb($alpha, 255, 50, 180))
        }
    }
}

foreach ($dir in $outputDirs) {
    $p = Join-Path $dir "vfx_bullet_decade.png"
    $bulletBmp.Save($p, [System.Drawing.Imaging.ImageFormat]::Png)
    Write-Host "Saved: $p"
}
$bulletBmp.Dispose()

Write-Host "All VFX assets successfully generated and saved!"
