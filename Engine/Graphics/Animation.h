#pragma once
#include <windows.h>
#include <vector>
#include "Engine/Graphics/Sprite.h"

struct AnimationFrame {
    RECT rect;
    float duration;
};

class Animation {
private:
    Sprite* m_sprite;
    std::vector<AnimationFrame> m_frames;
    bool m_isLoop;

    int m_currentFrameIndex;
    float m_timer;
    bool m_isFinished;

public:
    Animation(Sprite* sprite, bool isLoop = true);
    ~Animation();

    void AddFrame(const RECT& rect, float duration);
    void Update(float dt);
    void Render(LPD3DXSPRITE spriteHandler, float x, float y, bool flipX = false, float scale = 1.0f, D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255), float rotation = 0.0f);

    void Reset();
    void SetFrame(int index);
    bool IsFinished() const { return m_isFinished; }
    int GetCurrentFrameIndex() const { return m_currentFrameIndex; }
    int GetFrameCount() const { return (int)m_frames.size(); }
    RECT GetCurrentFrameRect() const;
};
