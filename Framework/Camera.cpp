#include "Camera.h"
#include <algorithm>

Camera::Camera(int screenW, int screenH)
    : m_x(0.0f), m_y(0.0f), m_screenWidth(screenW), m_screenHeight(screenH),
      m_mapLimitWidth(screenW), m_mapLimitHeight(screenH) {
}

void Camera::SetMapLimits(int mapW, int mapH) {
    m_mapLimitWidth = mapW;
    m_mapLimitHeight = mapH;
}

void Camera::Update(float targetCenterX, float targetCenterY) {
    // Keep target at horizontal center
    m_x = targetCenterX - (m_screenWidth * 0.5f);
    m_y = targetCenterY - (m_screenHeight * 0.5f);

    // Clamp horizontal boundaries
    float maxCamX = (float)(m_mapLimitWidth - m_screenWidth);
    if (maxCamX < 0) maxCamX = 0;
    m_x = std::max(0.0f, std::min(m_x, maxCamX));

    // Clamp vertical boundaries
    float maxCamY = (float)(m_mapLimitHeight - m_screenHeight);
    if (maxCamY < 0) maxCamY = 0;
    m_y = std::max(0.0f, std::min(m_y, maxCamY));
}
