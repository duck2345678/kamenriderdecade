#pragma once
#include "Engine/Core/Constants.h"
#include "Engine/Physics/SweptAABB.h"
#include <d3d9.h>
#include <d3dx9.h>
class Camera;

class GameObject {
protected:
    float m_x;
    float m_y;
    float m_width;
    float m_height;
    float m_vx;
    float m_vy;
    bool m_isAlive;
    ObjectType m_type;

public:
    GameObject(ObjectType type, float x = 0, float y = 0, float w = 0, float h = 0)
        : m_type(type), m_x(x), m_y(y), m_width(w), m_height(h), m_vx(0), m_vy(0), m_isAlive(true) {}

    virtual ~GameObject() = default;

    virtual void Update(float dt) = 0;
    virtual void Render(LPD3DXSPRITE spriteHandler, const Camera* camera) = 0;

    virtual Box GetBoundingBox() const {
        return Box(m_x, m_y, m_width, m_height, m_vx, m_vy);
    }

    // Getters & Setters
    float GetX() const { return m_x; }
    float GetY() const { return m_y; }
    void SetPosition(float x, float y) { m_x = x; m_y = y; }

    float GetVx() const { return m_vx; }
    float GetVy() const { return m_vy; }
    void SetVelocity(float vx, float vy) { m_vx = vx; m_vy = vy; }

    bool IsAlive() const { return m_isAlive; }
    void SetAlive(bool alive) { m_isAlive = alive; }

    ObjectType GetType() const { return m_type; }
};
