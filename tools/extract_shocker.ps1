$csharpCode = @"
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

public class ShockerExtractor {
    public static void CropRowTransparent(
        string sourceImagePath,
        int startY,
        int height,
        string outputImagePath,
        int threshold = 242)
    {
        using (Bitmap src = new Bitmap(sourceImagePath)) {
            int w = src.Width;
            int clampedHeight = Math.Min(height, src.Height - startY);
            
            // Find horizontal bounds (minX and maxX)
            int minX = w;
            int maxX = 0;
            
            for (int y = startY; y < startY + clampedHeight; y++) {
                for (int x = 0; x < w; x++) {
                    Color c = src.GetPixel(x, y);
                    // Non-white pixel
                    if (c.R < threshold || c.G < threshold || c.B < threshold) {
                        if (x < minX) minX = x;
                        if (x > maxX) maxX = x;
                    }
                }
            }
            
            if (minX > maxX) {
                Console.WriteLine("No content found in Y=" + startY);
                return;
            }
            
            minX = Math.Max(0, minX - 4);
            maxX = Math.Min(w - 1, maxX + 4);
            int cropWidth = maxX - minX + 1;
            
            using (Bitmap dest = new Bitmap(cropWidth, clampedHeight, PixelFormat.Format32bppArgb)) {
                BitmapData srcData = src.LockBits(
                    new Rectangle(minX, startY, cropWidth, clampedHeight),
                    ImageLockMode.ReadOnly,
                    PixelFormat.Format32bppArgb
                );
                
                BitmapData destData = dest.LockBits(
                    new Rectangle(0, 0, cropWidth, clampedHeight),
                    ImageLockMode.WriteOnly,
                    PixelFormat.Format32bppArgb
                );
                
                int stride = srcData.Stride;
                byte[] bytes = new byte[stride * clampedHeight];
                Marshal.Copy(srcData.Scan0, bytes, 0, bytes.Length);
                
                for (int y = 0; y < clampedHeight; y++) {
                    for (int x = 0; x < cropWidth; x++) {
                        int idx = y * stride + x * 4;
                        byte b = bytes[idx];
                        byte g = bytes[idx + 1];
                        byte r = bytes[idx + 2];
                        
                        // If background is white or near-white, make transparent
                        if (r >= threshold && g >= threshold && b >= threshold) {
                            bytes[idx + 3] = 0; // Alpha = 0
                        } else {
                            // Smooth alpha for near-threshold anti-aliased edge
                            int brightness = (r + g + b) / 3;
                            if (brightness > threshold - 15) {
                                float factor = (float)(255 - brightness) / 15.0f;
                                if (factor < 0) factor = 0;
                                if (factor > 1) factor = 1;
                                bytes[idx + 3] = (byte)(factor * 255);
                            } else {
                                bytes[idx + 3] = 255;
                            }
                        }
                    }
                }
                
                Marshal.Copy(bytes, 0, destData.Scan0, bytes.Length);
                src.UnlockBits(srcData);
                dest.UnlockBits(destData);
                
                dest.Save(outputImagePath, ImageFormat.Png);
                Console.WriteLine("Saved: " + outputImagePath + " (" + cropWidth + "x" + clampedHeight + ")");
            }
        }
    }
}
"@

Add-Type -TypeDefinition $csharpCode -ReferencedAssemblies "System.Drawing.dll"

$source = "c:\Users\ACER\Documents\GitHub\kamenriderdecade\2408160f-6e51-4757-9d36-72942b561cb9.png"

$outputDirs = @(
    "C:\Users\ACER\Documents\GitHub\kamenriderdecade\Assets\Textures",
    "C:\Users\ACER\.gemini\antigravity-ide\scratch\kamenriderdecade\Assets\Textures"
)

# Precisely tuned coordinates without text labels & numbers
$rows = @(
    @{ Name = "shocker_walk";   StartY = 95;  Height = 175 },
    @{ Name = "shocker_attack"; StartY = 345; Height = 210 },
    @{ Name = "shocker_hurt";   StartY = 645; Height = 175 },
    @{ Name = "shocker_die";    StartY = 915; Height = 170 }
)

Write-Host "Extracting Shocker Combatant animation strips..."

foreach ($r in $rows) {
    foreach ($dir in $outputDirs) {
        $outFile = Join-Path $dir ($r.Name + ".png")
        [ShockerExtractor]::CropRowTransparent($source, $r.StartY, $r.Height, $outFile, 245)
    }
}

# Also extract the standalone Explosion VFX from row 4 (rightmost frame)
# In row 4 (shocker_die), the explosion is roughly the rightmost quarter (x ~ 540 to 710)
Add-Type -AssemblyName System.Drawing
$bmp = New-Object System.Drawing.Bitmap("C:\Users\ACER\Documents\GitHub\kamenriderdecade\Assets\Textures\shocker_die.png")
$expWidth = 140
$expHeight = $bmp.Height
$expX = Math.Max(0, $bmp.Width - $expWidth)

$rect = New-Object System.Drawing.Rectangle($expX, 0, $expWidth, $expHeight)
$expBmp = $bmp.Clone($rect, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)

foreach ($dir in $outputDirs) {
    $expPath = Join-Path $dir "vfx_explosion.png"
    $expBmp.Save($expPath, [System.Drawing.Imaging.ImageFormat]::Png)
    Write-Host "Saved standalone VFX: $expPath"
}
$expBmp.Dispose()
$bmp.Dispose()

Write-Host "All Shocker Combatant assets processed successfully!"
