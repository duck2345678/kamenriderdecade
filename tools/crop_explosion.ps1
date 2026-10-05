Add-Type -AssemblyName System.Drawing

$diePath = "C:\Users\ACER\.gemini\antigravity-ide\scratch\kamenriderdecade\Assets\Textures\shocker_die.png"
$bmp = New-Object System.Drawing.Bitmap($diePath)

# Frame 4 (Explosion) is located at x=540 to 720
$rect = New-Object System.Drawing.Rectangle(540, 0, 180, $bmp.Height)
$expBmp = $bmp.Clone($rect, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)

$paths = @(
    "C:\Users\ACER\Documents\GitHub\kamenriderdecade\Assets\Textures\vfx_explosion.png",
    "C:\Users\ACER\.gemini\antigravity-ide\scratch\kamenriderdecade\Assets\Textures\vfx_explosion.png"
)

foreach ($p in $paths) {
    $expBmp.Save($p, [System.Drawing.Imaging.ImageFormat]::Png)
    Write-Host "Saved explosion to $p"
}

$expBmp.Dispose()
$bmp.Dispose()
