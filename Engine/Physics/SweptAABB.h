#pragma once
#include <algorithm>
#include <limits>

// Bounding Box Structure for Collision Detection
struct Box {
    float x;      // Top-left x
    float y;      // Top-left y
    float width;
    float height;
    float vx;     // Velocity x
    float vy;     // Velocity y

    Box() : x(0), y(0), width(0), height(0), vx(0), vy(0) {}
    Box(float _x, float _y, float _w, float _h, float _vx = 0, float _vy = 0)
        : x(_x), y(_y), width(_w), height(_h), vx(_vx), vy(_vy) {}

    float GetLeft() const { return x; }
    float GetRight() const { return x + width; }
    float GetTop() const { return y; }
    float GetBottom() const { return y + height; }
};

// Collision Result Information
struct CollisionResult {
    float collisionTime; // Normalized collision time in [0, 1]
    float normalX;       // Surface normal X (-1, 0, 1)
    float normalY;       // Surface normal Y (-1, 0, 1)

    CollisionResult() : collisionTime(1.0f), normalX(0), normalY(0) {}
};

class SweptAABB {
public:
    // Basic static AABB overlap check
    static bool CheckAABB(const Box& b1, const Box& b2) {
        return !(b1.GetRight() <= b2.GetLeft() ||
                 b1.GetLeft() >= b2.GetRight() ||
                 b1.GetBottom() <= b2.GetTop() ||
                 b1.GetTop() >= b2.GetBottom());
    }

    // Swept AABB algorithm for continuous collision detection (CCD)
    static CollisionResult CalculateSweptAABB(const Box& movingBox, const Box& staticBox, float dt) {
        CollisionResult result;

        // Relative velocity in frame time dt
        float dx = (movingBox.vx - staticBox.vx) * dt;
        float dy = (movingBox.vy - staticBox.vy) * dt;

        float invEntryX, invEntryY;
        float invExitX, invExitY;

        // Find distances between objects on near and far sides
        if (dx > 0.0f) {
            invEntryX = staticBox.GetLeft() - movingBox.GetRight();
            invExitX = staticBox.GetRight() - movingBox.GetLeft();
        } else {
            invEntryX = staticBox.GetRight() - movingBox.GetLeft();
            invExitX = staticBox.GetLeft() - movingBox.GetRight();
        }

        if (dy > 0.0f) {
            invEntryY = staticBox.GetTop() - movingBox.GetBottom();
            invExitY = staticBox.GetBottom() - movingBox.GetTop();
        } else {
            invEntryY = staticBox.GetBottom() - movingBox.GetTop();
            invExitY = staticBox.GetTop() - movingBox.GetBottom();
        }

        // Calculate entry and exit times along each axis
        float entryX, entryY;
        float exitX, exitY;

        entryX = (dx == 0.0f) ? -std::numeric_limits<float>::infinity() : invEntryX / dx;
        exitX = (dx == 0.0f) ? std::numeric_limits<float>::infinity() : invExitX / dx;

        entryY = (dy == 0.0f) ? -std::numeric_limits<float>::infinity() : invEntryY / dy;
        exitY = (dy == 0.0f) ? std::numeric_limits<float>::infinity() : invExitY / dy;

        // Find earliest and latest collision times
        float entryTime = std::max(entryX, entryY);
        float exitTime = std::min(exitX, exitY);

        // If no collision occurred
        if (entryTime > exitTime || (entryX < 0.0f && entryY < 0.0f) || entryX > 1.0f || entryY > 1.0f) {
            result.collisionTime = 1.0f;
            result.normalX = 0.0f;
            result.normalY = 0.0f;
            return result;
        }

        // Collision occurred, calculate normal
        result.collisionTime = entryTime;
        if (entryX > entryY) {
            result.normalX = (dx < 0.0f) ? 1.0f : -1.0f;
            result.normalY = 0.0f;
        } else {
            result.normalX = 0.0f;
            result.normalY = (dy < 0.0f) ? 1.0f : -1.0f;
        }

        return result;
    }
};
