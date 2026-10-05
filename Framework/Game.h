#pragma once
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "Constants.h"

class Decade;

class Game {
private:
    static Game* s_instance;
    HWND m_hWnd;
    HINSTANCE m_hInstance;

    LPDIRECT3D9 m_d3d;
    LPDIRECT3DDEVICE9 m_d3ddev;
    LPD3DXSPRITE m_spriteHandler;

    bool m_isRunning;
    float m_timeScale; // Used for Clock Up slow-motion effect!

    Decade* m_decade;

    Game();

public:
    static Game* GetInstance();
    ~Game();

    bool Initialize(HINSTANCE hInstance, int nCmdShow);
    void Run();
    void CleanUp();

    void Update(float dt);
    void Render();

    // Direct3D getters
    LPDIRECT3DDEVICE9 GetDevice() const { return m_d3ddev; }
    LPD3DXSPRITE GetSpriteHandler() const { return m_spriteHandler; }
    HWND GetWindowHandle() const { return m_hWnd; }

    // TimeScale getter & setter for Clock Up
    void SetTimeScale(float scale) { m_timeScale = scale; }
    float GetTimeScale() const { return m_timeScale; }

    Decade* GetPlayer() const { return m_decade; }

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
