#include "Game/World/TileMap.h"
#include <algorithm>

TileMap::TileMap()
    : m_tilesetTexture(nullptr), m_tilesetCols(24), m_tileSize(TILE_SIZE) {
}

void TileMap::Init(LPDIRECT3DDEVICE9 d3ddev) {
    m_tilesetTexture = TextureManager::GetInstance()->LoadTexture(d3ddev, L"Assets/Textures/tileset_city.png");
}

void TileMap::Render(LPD3DXSPRITE spriteHandler, const Camera* camera) {
    if (!m_tilesetTexture || !spriteHandler || !camera) return;

    const int* visual = GetMapVisualData();

    float camX = camera->GetX();
    float camY = camera->GetY();

    // Determine visible tile range (Frustum Culling)
    int startCol = std::max(0, (int)(camX / m_tileSize));
    int endCol   = std::min(MAP_WIDTH - 1, (int)((camX + camera->GetWidth()) / m_tileSize) + 1);

    int startRow = std::max(0, (int)(camY / m_tileSize));
    int endRow   = std::min(MAP_HEIGHT - 1, (int)((camY + camera->GetHeight()) / m_tileSize) + 1);

    D3DXMATRIX mat;
    for (int r = startRow; r <= endRow; ++r) {
        for (int c = startCol; c <= endCol; ++c) {
            int tileId = visual[r * MAP_WIDTH + c];
            if (tileId <= 0) continue;

            int tileIndex = tileId - 1;
            int srcCol = tileIndex % m_tilesetCols;
            int srcRow = tileIndex / m_tilesetCols;

            RECT srcRect = {
                srcCol * m_tileSize,
                srcRow * m_tileSize,
                (srcCol + 1) * m_tileSize,
                (srcRow + 1) * m_tileSize
            };

            float screenX = (c * m_tileSize) - camX;
            float screenY = (r * m_tileSize) - camY;

            D3DXMatrixIdentity(&mat);
            D3DXVECTOR2 translation(screenX, screenY);
            D3DXMatrixTransformation2D(&mat, NULL, 0.0f, NULL, NULL, 0.0f, &translation);

            spriteHandler->SetTransform(&mat);
            spriteHandler->Draw(m_tilesetTexture, &srcRect, NULL, NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
        }
    }

    D3DXMatrixIdentity(&mat);
    spriteHandler->SetTransform(&mat);
}

bool TileMap::IsSolidAt(float worldX, float worldY) const {
    if (worldX < 0 || worldX >= MAP_PIXEL_WIDTH || worldY < 0 || worldY >= MAP_PIXEL_HEIGHT) {
        return false;
    }

    int col = (int)(worldX / m_tileSize);
    int row = (int)(worldY / m_tileSize);

    if (row < 0 || row >= MAP_HEIGHT || col < 0 || col >= MAP_WIDTH) return false;

    const int* coll = GetMapCollisionData();
    return (coll[row * MAP_WIDTH + col] > 0);
}

bool TileMap::CheckGroundCollision(float x, float y, float width, float height, float& outGroundY) const {
    // Check points along the feet line: left edge, center, right edge
    float feetY = y + (height * 0.5f);
    float leftX = x - (width * 0.35f);
    float rightX = x + (width * 0.35f);
    float midX = x;

    // Check downwards by a small sensor step (up to 4 pixels)
    for (float checkY = feetY; checkY <= feetY + 6.0f; checkY += 2.0f) {
        if (IsSolidAt(leftX, checkY) || IsSolidAt(midX, checkY) || IsSolidAt(rightX, checkY)) {
            int row = (int)(checkY / m_tileSize);
            outGroundY = (row * m_tileSize) - (height * 0.5f);
            return true;
        }
    }

    return false;
}

bool TileMap::CheckHorizontalCollision(float x, float y, float width, float height, float vx, float& outCorrectedX) const {
    outCorrectedX = x;
    if (vx == 0.0f) return false;

    float checkX = (vx > 0) ? (x + width * 0.4f) : (x - width * 0.4f);
    float midY = y;
    float upperY = y - height * 0.3f;
    float lowerY = y + height * 0.3f;

    if (IsSolidAt(checkX, midY) || IsSolidAt(checkX, upperY) || IsSolidAt(checkX, lowerY)) {
        int col = (int)(checkX / m_tileSize);
        if (vx > 0) {
            outCorrectedX = (col * m_tileSize) - (width * 0.4f) - 0.5f;
        } else {
            outCorrectedX = ((col + 1) * m_tileSize) + (width * 0.4f) + 0.5f;
        }
        return true;
    }

    return false;
}
