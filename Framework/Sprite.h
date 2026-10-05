#pragma once
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

class Sprite {
private:
    LPDIRECT3DTEXTURE9 m_texture;
    int m_width;
    int m_height;

public:
    Sprite(LPDIRECT3DTEXTURE9 texture, int width = 0, int height = 0);
    ~Sprite();

    void Draw(
        LPD3DXSPRITE spriteHandler,
        float x,
        float y,
        const RECT* srcRect = nullptr,
        bool flipX = false,
        float scale = 1.0f,
        D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255)
    );

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
};
