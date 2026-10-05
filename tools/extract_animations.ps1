$csharpCode = @"
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

public class SpriteExtractor {
    public static void CropAndMakeTransparent(
        string sourceImagePath,
        int startY,
        int height,
        string outputImagePath,
        byte bgR, byte bgG, byte bgB) 
    {
        using (Bitmap src = new Bitmap(sourceImagePath)) {
            int w = src.Width;
            int clampedHeight = Math.Min(height, src.Height - startY);
            
            // First, find horizontal bounds (minX and maxX)
            int minX = w;
            int maxX = 0;
            
            for (int y = startY; y < startY + clampedHeight; y++) {
                for (int x = 0; x < w; x++) {
                    Color c = src.GetPixel(x, y);
                    // Check if pixel is not background
                    if (Math.Abs(c.R - bgR) > 5 || Math.Abs(c.G - bgG) > 5 || Math.Abs(c.B - bgB) > 5) {
                        if (x < minX) minX = x;
                        if (x > maxX) maxX = x;
                    }
                }
            }
            
            if (minX > maxX) {
                Console.WriteLine("No content in range Y=" + startY + " to " + (startY + clampedHeight));
                return;
            }
            
            // Add padding of 5px
            minX = Math.Max(0, minX - 5);
            maxX = Math.Min(w - 1, maxX + 5);
            int cropWidth = maxX - minX + 1;
            
            // Create 32-bit ARGB bitmap
            using (Bitmap dest = new Bitmap(cropWidth, clampedHeight, PixelFormat.Format32bppArgb)) {
                BitmapData srcData = src.LockBits(
                    new Rectangle(minX, startY, cropWidth, clampedHeight),
                    ImageLockMode.ReadOnly,
                    PixelFormat.Format24bppRgb
                );
                
                BitmapData destData = dest.LockBits(
                    new Rectangle(0, 0, cropWidth, clampedHeight),
                    ImageLockMode.WriteOnly,
                    PixelFormat.Format32bppArgb
                );
                
                int srcStride = srcData.Stride;
                int destStride = destData.Stride;
                
                byte[] srcBytes = new byte[srcStride * clampedHeight];
                byte[] destBytes = new byte[destStride * clampedHeight];
                
                Marshal.Copy(srcData.Scan0, srcBytes, 0, srcBytes.Length);
                
                for (int y = 0; y < clampedHeight; y++) {
                    for (int x = 0; x < cropWidth; x++) {
                        int srcIdx = y * srcStride + x * 3;
                        int destIdx = y * destStride + x * 4;
                        
                        byte b = srcBytes[srcIdx];
                        byte g = srcBytes[srcIdx + 1];
                        byte r = srcBytes[srcIdx + 2];
                        
                        // Check if green background
                        if (Math.Abs(r - bgR) <= 8 && Math.Abs(g - bgG) <= 8 && Math.Abs(b - bgB) <= 8) {
                            destBytes[destIdx] = 0;     // B
                            destBytes[destIdx + 1] = 0; // G
                            destBytes[destIdx + 2] = 0; // R
                            destBytes[destIdx + 3] = 0; // Alpha = Transparent!
                        } else {
                            destBytes[destIdx] = b;
                            destBytes[destIdx + 1] = g;
                            destBytes[destIdx + 2] = r;
                            destBytes[destIdx + 3] = 255; // Alpha = Opaque
                        }
                    }
                }
                
                Marshal.Copy(destBytes, 0, destData.Scan0, destBytes.Length);
                
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

$source = "C:\Users\ACER\Documents\GitHub\kamenriderdecade\kamen_rider_decade_sprite_sheet_by_neomar654_dgodxu4.png"

$outputDirs = @(
    "C:\Users\ACER\Documents\GitHub\kamenriderdecade\Assets\Textures",
    "C:\Users\ACER\.gemini\antigravity-ide\scratch\kamenriderdecade\Assets\Textures"
)

foreach ($dir in $outputDirs) {
    if (-not (Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
    }
}

# Row mappings identified from scan
$animations = @(
    @{ Name = "decade_idle";           StartY = 550;  Height = 130 },
    @{ Name = "decade_walk";           StartY = 715;  Height = 125 },
    @{ Name = "decade_run";            StartY = 860;  Height = 128 },
    @{ Name = "decade_jump";           StartY = 1008; Height = 125 },
    @{ Name = "decade_crouch";         StartY = 1168; Height = 128 },
    @{ Name = "decade_attack_light";   StartY = 1328; Height = 135 },
    @{ Name = "decade_attack_kick";    StartY = 1500; Height = 140 },
    @{ Name = "decade_gun_shoot";      StartY = 1800; Height = 140 },
    @{ Name = "decade_dimension_kick"; StartY = 7020; Height = 240 },
    @{ Name = "decade_special_slash";  StartY = 8290; Height = 180 }
)

Write-Host "Extracting and converting sprites to transparent 32-bit PNG..."

foreach ($anim in $animations) {
    foreach ($dir in $outputDirs) {
        $outFile = Join-Path $dir ($anim.Name + ".png")
        [SpriteExtractor]::CropAndMakeTransparent($source, $anim.StartY, $anim.Height, $outFile, 0, 102, 0)
    }
}

Write-Host "All animation strips extracted successfully!"
