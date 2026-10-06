#pragma once
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <unordered_map>

class TextureManager {
private:
    static TextureManager* s_instance;
    std::unordered_map<std::wstring, LPDIRECT3DTEXTURE9> m_textures;

    TextureManager() {}

public:
    static TextureManager* GetInstance();
    ~TextureManager();

    // Loads texture from file path (or returns cached pointer)
    LPDIRECT3DTEXTURE9 LoadTexture(LPDIRECT3DDEVICE9 d3ddev, const std::wstring& filePath, D3DCOLOR colorKey = 0);

    // Releases all cached textures
    void Clear();
};
