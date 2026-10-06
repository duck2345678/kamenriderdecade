Add-Type -AssemblyName System.Drawing

$file = "c:\Users\ACER\Documents\GitHub\kamenriderdecade\2408160f-6e51-4757-9d36-72942b561cb9.png"
$bmp = New-Object System.Drawing.Bitmap($file)

# Let's crop the first 3 labels (y=50..75, y=305..330, y=600..625) and see what's in there
$rect = New-Object System.Drawing.Rectangle(0, 0, [Math]::Min(724, $bmp.Width), 300)
$dest = $bmp.Clone($rect, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
$previewPath = "C:\Users\ACER\.gemini\antigravity-ide\scratch\kamenriderdecade\Assets\Textures\enemy_preview.png"
$dest.Save($previewPath, [System.Drawing.Imaging.ImageFormat]::Png)
$dest.Dispose()
$bmp.Dispose()
Write-Host "Enemy preview saved to $previewPath"
