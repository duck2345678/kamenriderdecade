#include "Engine/Graphics/Camera.h"
#include <algorithm>

Camera::Camera(int screenW, int screenH)
    : m_x(0.0f), m_y(0.0f), m_screenWidth(screenW), m_screenHeight(screenH),
      m_mapLimitWidth(screenW), m_mapLimitHeight(screenH) {
}

void Camera::SetMapLimits(int mapW, int mapH) {
    m_mapLimitWidth = mapW;
    m_mapLimitHeight = mapH;
}

void Camera::Update(float targetCenterX, float targetCenterY, float dt) {
    // Dead zone: the camera only scrolls once Decade nears a screen edge,
    // and he stays left of center so the path ahead is what gets revealed.
    float desiredX = m_x;
    const float screenX = targetCenterX - m_x;
    const float rightEdge = m_screenWidth * 0.46f;
    const float leftEdge = m_screenWidth * 0.28f;
    if (screenX > rightEdge) desiredX = targetCenterX - rightEdge;
    else if (screenX < leftEdge) desiredX = targetCenterX - leftEdge;

    float desiredY = m_y;
    const float screenY = targetCenterY - m_y;
    const float bottomEdge = m_screenHeight * 0.62f;
    const float topEdge = m_screenHeight * 0.38f;
    if (screenY > bottomEdge) desiredY = targetCenterY - bottomEdge;
    else if (screenY < topEdge) desiredY = targetCenterY - topEdge;

    float t = dt * 8.0f;
    if (t > 1.0f) t = 1.0f;
    m_x += (desiredX - m_x) * t;
    m_y += (desiredY - m_y) * t;

    float maxCamX = (float)(m_mapLimitWidth - m_screenWidth);
    if (maxCamX < 0) maxCamX = 0;
    m_x = std::max(0.0f, std::min(m_x, maxCamX));

    float maxCamY = (float)(m_mapLimitHeight - m_screenHeight);
    if (maxCamY < 0) maxCamY = 0;
    m_y = std::max(0.0f, std::min(m_y, maxCamY));
}
