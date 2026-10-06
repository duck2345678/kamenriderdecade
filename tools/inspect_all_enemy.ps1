Add-Type -AssemblyName System.Drawing

$file = "c:\Users\ACER\Documents\GitHub\kamenriderdecade\2408160f-6e51-4757-9d36-72942b561cb9.png"
$bmp = New-Object System.Drawing.Bitmap($file)

# The image is 724 x 2172
# Let's inspect the labels and height ranges
Write-Host "Image size: $($bmp.Width) x $($bmp.Height)"
$bmp.Dispose()
