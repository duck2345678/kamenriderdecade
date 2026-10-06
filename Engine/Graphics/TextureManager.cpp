#include "Engine/Graphics/TextureManager.h"

TextureManager* TextureManager::s_instance = nullptr;

TextureManager* TextureManager::GetInstance() {
    if (!s_instance) {
        s_instance = new TextureManager();
    }
    return s_instance;
}

TextureManager::~TextureManager() {
    Clear();
}

LPDIRECT3DTEXTURE9 TextureManager::LoadTexture(LPDIRECT3DDEVICE9 d3ddev, const std::wstring& filePath, D3DCOLOR colorKey) {
    auto it = m_textures.find(filePath);
    if (it != m_textures.end()) {
        return it->second;
    }

    D3DXIMAGE_INFO info;
    HRESULT hr = D3DXGetImageInfoFromFileW(filePath.c_str(), &info);
    if (FAILED(hr)) {
        return nullptr;
    }

    LPDIRECT3DTEXTURE9 texture = nullptr;
    hr = D3DXCreateTextureFromFileExW(
        d3ddev,
        filePath.c_str(),
        info.Width,
        info.Height,
        1,
        D3DPOOL_DEFAULT,
        D3DFMT_A8R8G8B8,
        D3DPOOL_DEFAULT,
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        colorKey,
        &info,
        NULL,
        &texture
    );

    if (FAILED(hr)) {
        return nullptr;
    }

    m_textures[filePath] = texture;
    return texture;
}

void TextureManager::Clear() {
    for (auto& pair : m_textures) {
        if (pair.second) {
            pair.second->Release();
            pair.second = nullptr;
        }
    }
    m_textures.clear();
}
