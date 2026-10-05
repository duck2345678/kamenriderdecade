#include "Sprite.h"

Sprite::Sprite(LPDIRECT3DTEXTURE9 texture, int width, int height)
    : m_texture(texture), m_width(width), m_height(height) {
}

Sprite::~Sprite() {
    // Texture memory is managed by TextureManager
    m_texture = nullptr;
}

void Sprite::Draw(
    LPD3DXSPRITE spriteHandler,
    float x,
    float y,
    const RECT* srcRect,
    bool flipX,
    float scale,
    D3DCOLOR color
) {
    if (!spriteHandler || !m_texture) return;

    D3DXMATRIX mat;
    D3DXMatrixIdentity(&mat);

    // Calculate dimensions
    int frameW = m_width;
    int frameH = m_height;
    if (srcRect) {
        frameW = srcRect->right - srcRect->left;
        frameH = srcRect->bottom - srcRect->top;
    }

    // Origin around center bottom (for platformer character feet grounding)
    D3DXVECTOR2 center((float)frameW * 0.5f, (float)frameH * 0.5f);
    D3DXVECTOR2 scaling(flipX ? -scale : scale, scale);
    D3DXVECTOR2 translation(x, y);

    D3DXMatrixTransformation2D(
        &mat,
        &center,
        0.0f,
        &scaling,
        NULL,
        0.0f,
        &translation
    );

    spriteHandler->SetTransform(&mat);
    spriteHandler->Draw(m_texture, srcRect, NULL, NULL, color);

    // Reset matrix
    D3DXMatrixIdentity(&mat);
    spriteHandler->SetTransform(&mat);
}
