#pragma once
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "Engine/Graphics/Camera.h"
#include "Engine/Graphics/TextureManager.h"

class ParallaxBackground {
private:
    LPDIRECT3DTEXTURE9 m_texFar;
    LPDIRECT3DTEXTURE9 m_texMid;
    LPDIRECT3DTEXTURE9 m_texNear;

    int m_farW, m_farH;
    int m_midW, m_midH;
    int m_nearW, m_nearH;

    void DrawTiledLayer(LPD3DXSPRITE spriteHandler, LPDIRECT3DTEXTURE9 tex, int texW, int texH, float scrollX, float screenY, float scale = 2.0f);

public:
    ParallaxBackground();
    ~ParallaxBackground() {}

    void Init(LPDIRECT3DDEVICE9 d3ddev);
    void Render(LPD3DXSPRITE spriteHandler, const Camera* camera);
};
