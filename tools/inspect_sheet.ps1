Add-Type -AssemblyName System.Drawing

$imagePath = "C:\Users\ACER\Documents\GitHub\kamenriderdecade\kamen_rider_decade_sprite_sheet_by_neomar654_dgodxu4.png"
if (-not (Test-Path $imagePath)) {
    Write-Host "File not found: $imagePath"
    exit 1
}

$bmp = New-Object System.Drawing.Bitmap($imagePath)
$bgR = 0
$bgG = 102
$bgB = 0
$w = $bmp.Width
$maxH = [Math]::Min(5000, $bmp.Height)

Write-Host "Scanning top $maxH rows of image ($w x $($bmp.Height))..."

$blocks = @()
$inBlock = $false
$start = 0

for ($y = 0; $y -lt $maxH; $y += 4) {
    $hasPixel = $false
    for ($x = 0; $x -lt $w; $x += 16) {
        $c = $bmp.GetPixel($x, $y)
        if ($c.R -ne $bgR -or $c.G -ne $bgG -or $c.B -ne $bgB) {
            $hasPixel = $true
            break
        }
    }

    if ($hasPixel -and -not $inBlock) {
        $inBlock = $true
        $start = $y
    } elseif (-not $hasPixel -and $inBlock) {
        $inBlock = $false
        $h = $y - $start
        if ($h -ge 30) {
            Write-Host "Block found: Y=$start to $y (Height: $h)"
            $blocks += [PSCustomObject]@{ Start = $start; End = $y; Height = $h }
        }
    }
}

$bmp.Dispose()
Write-Host "Total content blocks found in top $maxH rows: $($blocks.Count)"
