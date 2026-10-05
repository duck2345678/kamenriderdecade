$csharpCode = @"
using System;
using System.Drawing;
using System.Drawing.Imaging;

public class BossExtractorExact {
    public static void CropRow(
        string sourceImagePath,
        int startY,
        int height,
        string outputImagePath)
    {
        using (Bitmap src = new Bitmap(sourceImagePath)) {
            int w = src.Width;
            int clampedH = Math.Min(height, src.Height - startY);
            
            // Find horizontal bounds (minX and maxX) based on non-transparent pixels (Alpha > 20)
            int minX = w;
            int maxX = 0;
            
            for (int y = startY; y < startY + clampedH; y++) {
                for (int x = 0; x < w; x++) {
                    Color c = src.GetPixel(x, y);
                    if (c.A > 25) {
                        if (x < minX) minX = x;
                        if (x > maxX) maxX = x;
                    }
                }
            }
            
            if (minX > maxX) {
                Console.WriteLine("No content in range Y=" + startY);
                return;
            }
            
            minX = Math.Max(0, minX - 4);
            maxX = Math.Min(w - 1, maxX + 4);
            int cropWidth = maxX - minX + 1;
            
            Rectangle rect = new Rectangle(minX, startY, cropWidth, clampedH);
            using (Bitmap dest = src.Clone(rect, PixelFormat.Format32bppArgb)) {
                dest.Save(outputImagePath, ImageFormat.Png);
                Console.WriteLine("Saved Boss Anim: " + outputImagePath + " (" + cropWidth + "x" + clampedH + ")");
            }
        }
    }
}
"@

Add-Type -TypeDefinition $csharpCode -ReferencedAssemblies "System.Drawing.dll"

$source = "c:\Users\ACER\Documents\GitHub\kamenriderdecade\69fbd723-d664-4d50-93c0-308b39688126.png"

$outputDirs = @(
    "C:\Users\ACER\Documents\GitHub\kamenriderdecade\Assets\Textures",
    "C:\Users\ACER\.gemini\antigravity-ide\scratch\kamenriderdecade\Assets\Textures"
)

# Exact Y ranges measured to strictly exclude text labels and bottom numbers
$animations = @(
    @{ Name = "boss_ainz_idle";        StartY = 24;   Height = 74 },
    @{ Name = "boss_ainz_move";        StartY = 155;  Height = 74 },
    @{ Name = "boss_ainz_melee";       StartY = 278;  Height = 72 },
    @{ Name = "boss_ainz_teleport";    StartY = 384;  Height = 72 },
    @{ Name = "boss_ainz_magic_arrow"; StartY = 488;  Height = 70 },
    @{ Name = "boss_ainz_fireball";    StartY = 592;  Height = 70 },
    @{ Name = "boss_ainz_barrier";     StartY = 782;  Height = 145 },
    @{ Name = "boss_ainz_ultimate";    StartY = 1354; Height = 72 },
    @{ Name = "boss_ainz_hurt";        StartY = 1762; Height = 56 },
    @{ Name = "boss_ainz_defeat";      StartY = 1860; Height = 54 },
    @{ Name = "boss_ainz_enraged";     StartY = 1955; Height = 74 }
)

Write-Host "Extracting Ainz Ooal Gown Boss animations with exact bounds..."

foreach ($a in $animations) {
    foreach ($dir in $outputDirs) {
        $outFile = Join-Path $dir ($a.Name + ".png")
        [BossExtractorExact]::CropRow($source, $a.StartY, $a.Height, $outFile)
    }
}

Write-Host "All Boss animations extracted perfectly!"
