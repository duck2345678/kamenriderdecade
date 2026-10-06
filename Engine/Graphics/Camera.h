#pragma once
#include <windows.h>
#include "Engine/Core/Constants.h"

class Camera {
private:
    float m_x;
    float m_y;
    int m_screenWidth;
    int m_screenHeight;
    int m_mapLimitWidth;
    int m_mapLimitHeight;

public:
    Camera(int screenW = SCREEN_WIDTH, int screenH = SCREEN_HEIGHT);
    ~Camera() {}

    void SetMapLimits(int mapW, int mapH);
    void Update(float targetCenterX, float targetCenterY, float dt);

    float GetX() const { return m_x; }
    float GetY() const { return m_y; }
    int GetWidth() const { return m_screenWidth; }
    int GetHeight() const { return m_screenHeight; }

    // World to Screen conversion
    void WorldToScreen(float worldX, float worldY, float& screenX, float& screenY) const {
        screenX = worldX - m_x;
        screenY = worldY - m_y;
    }
};
