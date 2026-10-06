#include "Game/World/ParallaxBackground.h"
#include <cmath>

ParallaxBackground::ParallaxBackground()
    : m_texFar(nullptr), m_texMid(nullptr), m_texNear(nullptr),
      m_farW(384), m_farH(224), m_midW(384), m_midH(224), m_nearW(1009), m_nearH(224) {
}

void ParallaxBackground::Init(LPDIRECT3DDEVICE9 d3ddev) {
    TextureManager* tm = TextureManager::GetInstance();
    m_texFar = tm->LoadTexture(d3ddev, L"Assets/Textures/bg_skyline_far.png");
    m_texMid = tm->LoadTexture(d3ddev, L"Assets/Textures/bg_skyline_mid.png");
    m_texNear = tm->LoadTexture(d3ddev, L"Assets/Textures/bg_buildings_near.png");
}

void ParallaxBackground::DrawTiledLayer(
    LPD3DXSPRITE spriteHandler,
    LPDIRECT3DTEXTURE9 tex,
    int texW,
    int texH,
    float scrollX,
    float screenY,
    float scale
) {
    if (!tex || !spriteHandler) return;

    float scaledW = texW * scale;
    float startX = -std::fmod(scrollX, scaledW);
    if (startX > 0) startX -= scaledW;

    D3DXMATRIX mat;
    while (startX < SCREEN_WIDTH) {
        D3DXMatrixIdentity(&mat);
        D3DXVECTOR2 scaling(scale, scale);
        D3DXVECTOR2 translation(startX, screenY);

        D3DXMatrixTransformation2D(&mat, NULL, 0.0f, &scaling, NULL, 0.0f, &translation);
        spriteHandler->SetTransform(&mat);
        spriteHandler->Draw(tex, NULL, NULL, NULL, D3DCOLOR_ARGB(255, 255, 255, 255));

        startX += scaledW;
    }

    D3DXMatrixIdentity(&mat);
    spriteHandler->SetTransform(&mat);
}

void ParallaxBackground::Render(LPD3DXSPRITE spriteHandler, const Camera* camera) {
    if (!camera) return;

    float camX = camera->GetX();

    // 1. Far skyline (scrolls at 10% speed)
    DrawTiledLayer(spriteHandler, m_texFar, m_farW, m_farH, camX * 0.1f, 0.0f, 2.5f);

    // 2. Mid skyline (scrolls at 30% speed)
    DrawTiledLayer(spriteHandler, m_texMid, m_midW, m_midH, camX * 0.3f, 50.0f, 2.5f);

    // 3. Near dark buildings (scrolls at 50% speed)
    DrawTiledLayer(spriteHandler, m_texNear, m_nearW, m_nearH, camX * 0.5f, 100.0f, 2.5f);
}
