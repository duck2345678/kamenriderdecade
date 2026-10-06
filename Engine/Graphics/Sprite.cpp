#include "Engine/Graphics/Sprite.h"

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
    D3DCOLOR color,
    float rotation
) {
    DrawEx(spriteHandler, x, y, srcRect, flipX ? -scale : scale, scale, 0.0f, rotation, color);
}

void Sprite::DrawEx(
    LPD3DXSPRITE spriteHandler,
    float x,
    float y,
    const RECT* srcRect,
    float scaleX,
    float scaleY,
    float shearX,
    float rotation,
    D3DCOLOR color
) {
    if (!spriteHandler || !m_texture) return;

    int frameW = m_width;
    int frameH = m_height;
    if (srcRect) {
        frameW = srcRect->right - srcRect->left;
        frameH = srcRect->bottom - srcRect->top;
    }

    // (x, y) is where the center of the source rect lands on screen.
    // Order: move center to origin -> scale/flip -> shear -> rotate -> move to (x, y).
    const float fw = (float)frameW;
    const float fh = (float)frameH;
    D3DXMATRIX toOrigin, scaling, shear, rot, toPos, mat;
    D3DXMatrixTranslation(&toOrigin, -fw * 0.5f, -fh * 0.5f, 0.0f);
    D3DXMatrixScaling(&scaling, scaleX, scaleY, 1.0f);
    D3DXMatrixIdentity(&shear);
    shear._21 = shearX;
    D3DXMatrixRotationZ(&rot, rotation);
    D3DXMatrixTranslation(&toPos, x, y, 0.0f);
    mat = toOrigin * scaling * shear * rot * toPos;

    spriteHandler->SetTransform(&mat);
    spriteHandler->Draw(m_texture, srcRect, NULL, NULL, color);

    D3DXMatrixIdentity(&mat);
    spriteHandler->SetTransform(&mat);
}
