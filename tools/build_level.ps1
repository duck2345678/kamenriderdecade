# Builds a side-scrolling stage from the city tileset and writes WorldMapData.h.
# Solid caps: 272 left, 273 mid, 274 right. Ground fill: 51.
$W = 176
$H = 30
$Ground = 24
$visual = New-Object int[] ($W * $H)
$coll = New-Object int[] ($W * $H)

function Set-Cell([int]$c, [int]$r, [int]$id, [bool]$solid) {
    if ($c -lt 0 -or $c -ge $W -or $r -lt 0 -or $r -ge $H) { return }
    $script:visual[$r * $W + $c] = $id
    if ($solid) { $script:coll[$r * $W + $c] = 1 }
}

function Paint([int]$c, [int]$r, [int]$id) {
    if ($c -lt 0 -or $c -ge $W -or $r -lt 0 -or $r -ge $H) { return }
    if ($script:coll[$r * $W + $c] -ne 0) { return }
    if ($script:visual[$r * $W + $c] -ne 0) { return }
    $script:visual[$r * $W + $c] = $id
}

function Place-Cap([int]$c0, [int]$c1, [int]$row, [bool]$fillDown) {
    for ($c = $c0; $c -le $c1; $c++) {
        $id = 273
        if ($c1 -gt $c0 -and $c -eq $c0) { $id = 272 }
        elseif ($c1 -gt $c0 -and $c -eq $c1) { $id = 274 }
        Set-Cell $c $row $id $true
        if ($fillDown) {
            for ($r = $row + 1; $r -lt $H; $r++) { Set-Cell $c $r 51 $true }
        }
    }
}

# --- Section 1: tutorial ground, one small gap, a low step ---
Place-Cap 0 21 $Ground $true
Place-Cap 24 39 $Ground $true
Place-Cap 30 34 21 $false

# --- Section 2: wider gaps, stepped platforms, a high reward line ---
Place-Cap 40 48 $Ground $true
Place-Cap 53 64 $Ground $true
Place-Cap 68 89 $Ground $true
Place-Cap 44 47 21 $false
Place-Cap 54 58 20 $false
Place-Cap 60 63 21 $false
Place-Cap 66 69 21 $false
Place-Cap 73 79 18 $false

# --- Section 3: enemy avenue on solid ground, a card platform above ---
Place-Cap 90 114 $Ground $true
Place-Cap 98 102 21 $false

# --- Section 4: vertical hops over a pit ---
# ground 110-114 already placed as part of 90-114. Pit starts at 115.
Place-Cap 116 119 22 $false
Place-Cap 122 125 20 $false
Place-Cap 128 131 22 $false
Place-Cap 133 141 $Ground $true

# --- Section 5: safe ground plus a harder upper branch ---
Place-Cap 142 160 $Ground $true
Place-Cap 138 141 21 $false
Place-Cap 144 148 18 $false
Place-Cap 152 156 18 $false

# --- Section 6: final hops, then a quiet run to the gate ---
Place-Cap 162 165 22 $false
Place-Cap 168 171 21 $false
Place-Cap 172 175 $Ground $true

# Borders so the rider cannot leave the stage.
for ($r = 16; $r -lt $H; $r++) { Set-Cell 0 $r 50 $true }
for ($r = 16; $r -lt $H; $r++) { Set-Cell 175 $r 50 $true }

# Scenery (never solid): distant windows and a checkpoint ladder.
foreach ($c in @(8, 36, 58, 84, 108, 150)) {
    Paint $c 9 77
    Paint $c 10 53
    Paint ($c + 1) 9 78
    Paint ($c + 1) 10 54
}
foreach ($c in @(12, 70, 120, 146)) {
    for ($r = 6; $r -le 14; $r++) { Paint $c $r 50 }
}
# Checkpoint marker just before the pit section.
Paint 110 21 147
Paint 110 22 147
Paint 110 23 147
# Goal gate: stacked green windows the rider walks into.
for ($r = 20; $r -le 23; $r++) { Paint 173 $r 53 }

# Reachability: surfaces are solid runs whose tile above is empty.
$surfaces = @()
for ($r = 0; $r -lt $H; $r++) {
    $c = 0
    while ($c -lt $W) {
        $aboveEmpty = ($r -eq 0) -or ($coll[($r - 1) * $W + $c] -eq 0)
        if ($coll[$r * $W + $c] -eq 1 -and $aboveEmpty) {
            $c0 = $c
            while ($c -lt $W -and $coll[$r * $W + $c] -eq 1 -and (($r -eq 0) -or ($coll[($r - 1) * $W + $c] -eq 0))) { $c++ }
            $surfaces += ,@($r, $c0, ($c - 1))
        } else { $c++ }
    }
}

function Gap([int]$a0, [int]$a1, [int]$b0, [int]$b1) {
    if ($a1 -ge $b0 -and $b1 -ge $a0) { return 0 }
    if ($b0 -gt $a1) { return ($b0 - $a1 - 1) }
    return ($a0 - $b1 - 1)
}

function Can-Jump($src, $dst) {
    $rise = $src[0] - $dst[0]
    $gap = Gap $src[1] $src[2] $dst[1] $dst[2]
    if ($rise -gt 4) { return $false }
    if ($rise -ge 4) { return ($gap -le 2) }
    if ($rise -eq 3) { return ($gap -le 3) }
    if ($rise -ge 0) { return ($gap -le 4) }
    return ($gap -le 5)
}

$start = -1
$goal = -1
for ($i = 0; $i -lt $surfaces.Count; $i++) {
    $s = $surfaces[$i]
    if ($s[0] -eq $Ground -and $s[1] -le 4 -and $s[2] -ge 4) { $start = $i }
    if ($s[0] -eq $Ground -and $s[1] -le 174 -and $s[2] -ge 174) { $goal = $i }
}
if ($start -lt 0 -or $goal -lt 0) { throw "start or goal surface missing (start=$start goal=$goal)" }

$seen = New-Object bool[] $surfaces.Count
$queue = New-Object System.Collections.Generic.Queue[int]
$queue.Enqueue($start)
$seen[$start] = $true
while ($queue.Count -gt 0) {
    $i = $queue.Dequeue()
    for ($j = 0; $j -lt $surfaces.Count; $j++) {
        if ($seen[$j]) { continue }
        if (Can-Jump $surfaces[$i] $surfaces[$j]) {
            $seen[$j] = $true
            $queue.Enqueue($j)
        }
    }
}
if (-not $seen[$goal]) { throw "goal is not reachable from the start" }
Write-Host "Reachable surfaces: $((@($seen | Where-Object { $_ })).Count) / $($surfaces.Count)"

$enemies = @(
    @{ col = 14;  left = 6;   right = 20;  row = 24 },
    @{ col = 28;  left = 25;  right = 36;  row = 24 },
    @{ col = 35;  left = 32;  right = 38;  row = 24 },
    @{ col = 44;  left = 41;  right = 47;  row = 24 },
    @{ col = 56;  left = 54;  right = 58;  row = 20 },
    @{ col = 61;  left = 54;  right = 63;  row = 24 },
    @{ col = 74;  left = 70;  right = 78;  row = 24 },
    @{ col = 77;  left = 73;  right = 79;  row = 18 },
    @{ col = 86;  left = 82;  right = 89;  row = 24 },
    @{ col = 93;  left = 90;  right = 96;  row = 24 },
    @{ col = 99;  left = 98;  right = 102; row = 21 },
    @{ col = 105; left = 102; right = 110; row = 24 },
    @{ col = 112; left = 108; right = 114; row = 24 },
    @{ col = 117; left = 116; right = 119; row = 22 },
    @{ col = 123; left = 122; right = 125; row = 20 },
    @{ col = 130; left = 128; right = 131; row = 22 },
    @{ col = 137; left = 134; right = 140; row = 24 },
    @{ col = 146; left = 144; right = 148; row = 18 },
    @{ col = 154; left = 152; right = 156; row = 18 },
    @{ col = 156; left = 150; right = 159; row = 24 },
    @{ col = 163; left = 162; right = 165; row = 22 },
    @{ col = 169; left = 168; right = 171; row = 21 }
)
foreach ($e in $enemies) {
    $c = [int]$e.col; $r = [int]$e.row
    if ($coll[$r * $W + $c] -ne 1) { throw "enemy at col $c stands on empty tile row $r" }
    if ($r -gt 0 -and $coll[($r - 1) * $W + $c] -ne 0) { throw "enemy at col $c is inside a block" }
    for ($pc = [int]$e.left; $pc -le [int]$e.right; $pc++) {
        if ($coll[$r * $W + $pc] -ne 1) { throw "enemy patrol crosses a hole at col $pc row $r" }
    }
}
Write-Host "Enemies stand on solid ground: $($enemies.Count)"

function Emit-Grid($arr) {
    $lines = @()
    for ($r = 0; $r -lt $H; $r++) {
        $row = @()
        for ($c = 0; $c -lt $W; $c++) { $row += [string]$arr[$r * $W + $c] }
        $lines += ("        " + ($row -join ", ") + $(if ($r -lt $H - 1) { "," } else { "" }))
    }
    return ($lines -join "`n")
}

$enemyLines = @()
for ($i = 0; $i -lt $enemies.Count; $i++) {
    $e = $enemies[$i]
    $x = ([int]$e.col + 0.5) * 16
    $left = [int]$e.left * 16 + 14
    $right = ([int]$e.right + 1) * 16 - 14
    $top = [int]$e.row * 16
    $comma = $(if ($i -lt $enemies.Count - 1) { "," } else { "" })
    $enemyLines += ('    { ' + ('{0:0.0}f' -f $x) + ', ' + ('{0:0.0}f' -f $left) + ', ' + ('{0:0.0}f' -f $right) + ', ' + ('{0:0.0}f' -f $top) + ' }' + $comma)
}

$pickups = @(
    @{ col = 76;  row = 18 },
    @{ col = 100; row = 21 },
    @{ col = 148; row = 18 },
    @{ col = 153; row = 18 }
)
$pickupLines = @()
for ($i = 0; $i -lt $pickups.Count; $i++) {
    $p = $pickups[$i]
    $r = [int]$p.row
    $c = [int]$p.col
    if ($coll[$r * $W + $c] -ne 1) { throw "pickup col $c is not on a platform" }
    $x = ($c + 0.5) * 16
    $y = $r * 16 - 30
    $comma = $(if ($i -lt $pickups.Count - 1) { "," } else { "" })
    $pickupLines += ('    { ' + ('{0:0.0}f' -f $x) + ', ' + ('{0:0.0}f' -f $y) + ' }' + $comma)
}

$spawnX = (4 + 0.5) * 16
$spawnY = $Ground * 16 - 24
$goalX = 173 * 16
$checkX = 110 * 16

$header = @"
// Side-scrolling stage laid out like a short Mario Bros. 3 course,
// built only from the city tileset already in the project.
// Sections run left to right:
//   0-39   tutorial ground, a 2-tile gap, one low step
//   40-89  wider gaps and a high reward line
//   90-114 enemy avenue
//   115-132 vertical hops over a pit
//   133-160 safe road plus an optional upper branch
//   161-175 final hops into the gate
#pragma once

constexpr int MAP_WIDTH = $W;
constexpr int MAP_HEIGHT = $H;
constexpr int TILE_SIZE = 16;
constexpr int MAP_PIXEL_WIDTH = MAP_WIDTH * TILE_SIZE;
constexpr int MAP_PIXEL_HEIGHT = MAP_HEIGHT * TILE_SIZE;

constexpr float STAGE_SPAWN_X = $('{0:0.0}f' -f $spawnX);
constexpr float STAGE_SPAWN_Y = $('{0:0.0}f' -f $spawnY);
constexpr float STAGE_GOAL_X = $('{0:0.0}f' -f $goalX);
constexpr float STAGE_CHECKPOINT_X = $('{0:0.0}f' -f $checkX);

struct EnemySpawn {
    float x;
    float left;
    float right;
    float surfaceTop;
};

struct PickupSpawn {
    float x;
    float y;
};

inline const EnemySpawn* GetEnemySpawns(int& count) {
    static const EnemySpawn spawns[] = {
$($enemyLines -join "`n")
    };
    count = (int)(sizeof(spawns) / sizeof(spawns[0]));
    return spawns;
}

inline const PickupSpawn* GetPickupSpawns(int& count) {
    static const PickupSpawn spawns[] = {
$($pickupLines -join "`n")
    };
    count = (int)(sizeof(spawns) / sizeof(spawns[0]));
    return spawns;
}

inline const int* GetMapVisualData() {
    static const int s_visualData[] = {
$(Emit-Grid $visual)
    };
    return s_visualData;
}

inline const int* GetMapCollisionData() {
    static const int s_collisionData[] = {
$(Emit-Grid $coll)
    };
    return s_collisionData;
}
"@

$out = "C:\Users\ACER\Documents\GitHub\kamenriderdecade\Game\World\WorldMapData.h"
Set-Content -Path $out -Value $header -Encoding UTF8
Write-Host "Wrote $out"
