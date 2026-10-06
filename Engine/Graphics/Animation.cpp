#include "Engine/Graphics/Animation.h"

Animation::Animation(Sprite* sprite, bool isLoop)
    : m_sprite(sprite), m_isLoop(isLoop), m_currentFrameIndex(0), m_timer(0.0f), m_isFinished(false) {
}

Animation::~Animation() {
    m_sprite = nullptr;
}

void Animation::AddFrame(const RECT& rect, float duration) {
    m_frames.push_back({ rect, duration });
}

void Animation::Update(float dt) {
    if (m_frames.empty() || m_isFinished) return;

    m_timer += dt;
    float currentDuration = m_frames[m_currentFrameIndex].duration;

    if (m_timer >= currentDuration) {
        m_timer -= currentDuration;
        m_currentFrameIndex++;

        if (m_currentFrameIndex >= (int)m_frames.size()) {
            if (m_isLoop) {
                m_currentFrameIndex = 0;
            } else {
                m_currentFrameIndex = (int)m_frames.size() - 1;
                m_isFinished = true;
            }
        }
    }
}

void Animation::Render(LPD3DXSPRITE spriteHandler, float x, float y, bool flipX, float scale, D3DCOLOR color, float rotation) {
    if (m_frames.empty() || !m_sprite) return;

    const RECT& srcRect = m_frames[m_currentFrameIndex].rect;
    m_sprite->Draw(spriteHandler, x, y, &srcRect, flipX, scale, color, rotation);
}

void Animation::Reset() {
    m_currentFrameIndex = 0;
    m_timer = 0.0f;
    m_isFinished = false;
}

void Animation::SetFrame(int index) {
    if (m_frames.empty()) return;
    if (index < 0) index = 0;
    if (index >= (int)m_frames.size()) index = (int)m_frames.size() - 1;
    m_currentFrameIndex = index;
    m_timer = 0.0f;
    m_isFinished = false;
}

RECT Animation::GetCurrentFrameRect() const {
    if (m_frames.empty()) {
        RECT r = { 0, 0, 0, 0 };
        return r;
    }
    return m_frames[m_currentFrameIndex].rect;
}
