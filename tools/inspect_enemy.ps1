Add-Type -AssemblyName System.Drawing

$file = "c:\Users\ACER\Documents\GitHub\kamenriderdecade\2408160f-6e51-4757-9d36-72942b561cb9.png"
if (-not (Test-Path $file)) {
    Write-Host "File not found!"
    exit 1
}

$bmp = New-Object System.Drawing.Bitmap($file)
Write-Host "Resolution: $($bmp.Width) x $($bmp.Height)"
Write-Host "PixelFormat: $($bmp.PixelFormat)"

$tl = $bmp.GetPixel(0, 0)
Write-Host "Top-left color: R=$($tl.R), G=$($tl.G), B=$($tl.B), A=$($tl.A)"

# Scan rows for content blocks
$bgR = $tl.R; $bgG = $tl.G; $bgB = $tl.B
$blocks = @()
$inBlock = $false
$start = 0

for ($y = 0; $y -lt $bmp.Height; $y += 2) {
    $hasContent = $false
    for ($x = 0; $x -lt $bmp.Width; $x += 4) {
        $p = $bmp.GetPixel($x, $y)
        if ($p.A -gt 10 -and ($p.R -ne $bgR -or $p.G -ne $bgG -or $p.B -ne $bgB)) {
            $hasContent = $true
            break
        }
    }
    if ($hasContent -and -not $inBlock) {
        $inBlock = $true
        $start = $y
    } elseif (-not $hasContent -and $inBlock) {
        $inBlock = $false
        $h = $y - $start
        if ($h -ge 15) {
            Write-Host "Row block: Y=$start to $y (Height: $h)"
        }
    }
}

$bmp.Dispose()
