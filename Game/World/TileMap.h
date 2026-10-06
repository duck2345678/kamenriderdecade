#pragma once
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "Engine/Graphics/Camera.h"
#include "Engine/Graphics/TextureManager.h"
#include "Game/World/WorldMapData.h"

class TileMap {
private:
    LPDIRECT3DTEXTURE9 m_tilesetTexture;
    int m_tilesetCols;
    int m_tileSize;

public:
    TileMap();
    ~TileMap() {}

    void Init(LPDIRECT3DDEVICE9 d3ddev);
    void Render(LPD3DXSPRITE spriteHandler, const Camera* camera);

    // Collision detection
    bool IsSolidAt(float worldX, float worldY) const;
    bool CheckGroundCollision(float x, float y, float width, float height, float& outGroundY) const;
    bool CheckHorizontalCollision(float x, float y, float width, float height, float vx, float& outCorrectedX) const;

    int GetMapWidth() const { return MAP_PIXEL_WIDTH; }
    int GetMapHeight() const { return MAP_PIXEL_HEIGHT; }
};
