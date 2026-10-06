Add-Type -AssemblyName System.Drawing

$file = "c:\Users\ACER\Documents\GitHub\kamenriderdecade\2408160f-6e51-4757-9d36-72942b561cb9.png"
$bmp = New-Object System.Drawing.Bitmap($file)

# Let's save a preview of each section
$sections = @(
    @{ Name = "row1_walk";   Y = 50;   H = 240 },
    @{ Name = "row2_act2";   Y = 300;  H = 280 },
    @{ Name = "row3_act3";   Y = 590;  H = 260 },
    @{ Name = "row4_act4";   Y = 860;  H = 250 },
    @{ Name = "row5_act5";   Y = 1130; H = 240 },
    @{ Name = "row6_act6";   Y = 1390; H = 250 },
    @{ Name = "row7_act7";   Y = 1650; H = 220 },
    @{ Name = "row8_act8";   Y = 1870; H = 290 }
)

foreach ($sec in $sections) {
    $rect = New-Object System.Drawing.Rectangle(0, $sec.Y, $bmp.Width, [Math]::Min($sec.H, $bmp.Height - $sec.Y))
    $dest = $bmp.Clone($rect, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $dest.Save("C:\Users\ACER\.gemini\antigravity-ide\scratch\kamenriderdecade\Assets\Textures\$($sec.Name).png", [System.Drawing.Imaging.ImageFormat]::Png)
    $dest.Dispose()
    Write-Host "Saved $($sec.Name)"
}

$bmp.Dispose()
