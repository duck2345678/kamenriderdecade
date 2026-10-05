#pragma once

// Game Screen Configurations
#define GAME_TITLE L"Kamen Rider Decade: Dimensional Convergence - SE102 UIT"
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FRAME_PER_SECOND 60
#define DEFAULT_DELTA_TIME (1.0f / FRAME_PER_SECOND)

// World & Physics Constants
#define GRAVITY 980.0f          // pixels / s^2
#define MAX_FALL_SPEED 600.0f   // pixels / s

// Game Zones
enum class GameZone {
    WorldOfKuuga = 1,
    WorldOfFaiz = 2,
    WorldOfKabuto = 3,
    WorldOfDenO = 4,
    WorldOfDestruction = 5
};

// Object Tags & Types
enum class ObjectType {
    Player,
    EnemyGrunt,
    EnemyWorm,
    Boss,
    Bullet,
    SlashHitbox,
    SolidGround,
    MovingPlatform,
    Slope,
    Hazard,
    ZoneTrigger,
    CardItem
};
