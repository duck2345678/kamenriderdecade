#pragma once
#include "Game/Entities/GameObject.h"
#include "Engine/Graphics/Animation.h"
#include "Game/World/TileMap.h"

class Decade;

// Grunt that patrols a platform. When Decade is on the same stretch of
// ground it closes in and swings, and it stops at the ledge instead of walking off.
class Shocker : public GameObject {
private:
    enum class State { Patrol, Chase, Attack, Hurt, Die };

    Animation* m_walk;
    Animation* m_attack;
    Animation* m_hurt;
    Animation* m_die;
    Sprite* m_walkSprite;
    Sprite* m_attackSprite;
    Sprite* m_hurtSprite;
    Sprite* m_dieSprite;

    float m_left;
    float m_right;
    float m_homeX;
    int m_dir;
    int m_hp;
    int m_lastHitSerial;
    float m_hitFromX;
    bool m_swingConnected;
    State m_state;
    float m_stateTime;

    bool HasFooting(const TileMap* tileMap, float x) const;
    bool Sees(float playerX, float playerY) const;
    void BeginAttack(float playerX);
    void SnapToGround(const TileMap* tileMap, float dt);
    void RenderAnim(LPD3DXSPRITE spriteHandler, const Camera* camera, Animation* anim, const float* feet, int feetCount) const;

public:
    Shocker(float x, float surfaceTop, float left, float right);
    virtual ~Shocker();

    void Init(LPDIRECT3DDEVICE9 device);
    void Reset();
    virtual void Update(float dt) override { Update(dt, nullptr, m_x, m_y); }
    void Update(float dt, const TileMap* tileMap, float playerX, float playerY);
    virtual void Render(LPD3DXSPRITE spriteHandler, const Camera* camera) override;

    bool IsTargetable() const { return m_isAlive && m_state != State::Die; }
    void TakeHit(int damage, int attackSerial, float fromX);
    bool AttackHits(float px, float py, float pw, float ph);
};

// Rider card floating above a platform. Touching it restores health.
class CardPickup : public GameObject {
private:
    Sprite* m_sprite;
    float m_baseY;
    float m_time;
    bool m_taken;

public:
    CardPickup(float x, float y);
    virtual ~CardPickup();

    void Init(LPDIRECT3DDEVICE9 device);
    void Reset();
    virtual void Update(float dt) override;
    virtual void Render(LPD3DXSPRITE spriteHandler, const Camera* camera) override;

    bool IsTaken() const { return m_taken; }
    void Take() { m_taken = true; }
};
