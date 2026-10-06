#pragma once
#include <windows.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "Game/Entities/GameObject.h"
#include "Engine/Graphics/Animation.h"
#include "Engine/Graphics/TextureManager.h"
#include "Engine/Core/Constants.h"

class Camera;
class TileMap;

enum class DecadeState {
    Idle,
    Walk,
    Run,
    Jump,
    Fall,
    Block,
    AttackPunch,   // Phím J
    AttackSlash,   // Phím K
    AttackShoot,   // Phím L
    DimensionKick, // Phím O
    Hurt,
    Defeat
};

class Decade : public GameObject {
private:
    DecadeState m_state;
    std::unordered_map<std::string, Animation*> m_animations;
    std::unordered_map<std::string, Sprite*> m_sprites;

    bool m_flipX;          // true = looking left, false = looking right
    bool m_isGrounded;
    int m_health;
    int m_maxHealth;

    // Dimension Kick tunnel cards. shatter < 0 means still intact.
    struct DimensionCard {
        float x, y;
        int style;
        float depth;    // 0 = nearest (first card), 1 = farthest
        float shatter;
    };
    std::vector<DimensionCard> m_dimensionCards;
    Sprite* m_cardSprite;
    float m_kickTime;
    float m_kickOriginX;
    float m_kickOriginY;
    float m_kickDir;
    float m_kickAngle;     // dive angle (radians, positive = downward along m_kickDir)
    bool m_kickScripted;

    struct Bullet {
        float x, y, vx;
        float life;
        bool active;
    };
    std::vector<Bullet> m_bullets;
    Sprite* m_bulletSprite;

    bool m_previousPunchDown;
    bool m_previousSlashDown;
    bool m_previousShootDown;
    bool m_previousKickDown;
    bool m_controlLocked;
    int m_attackSerial;

    void SetState(DecadeState newState);
    void BeginDimensionKick();
    void UpdateDimensionKick(float dt, const TileMap* tileMap);
    void SpawnBullet();
    std::string CurrentAnimName() const;
    float GetAnimScale(const std::string& anim) const;
    void GetSpriteAnchor(const std::string& anim, int frame, float frameW, float frameH, float& anchorX, float& anchorY) const;

public:
    Decade(float startX, float startY);
    virtual ~Decade();

    void InitAnimations(LPDIRECT3DDEVICE9 d3ddev);
    void Update(float dt, const TileMap* tileMap);
    virtual void Update(float dt) override { Update(dt, nullptr); }
    virtual void Render(LPD3DXSPRITE spriteHandler, const Camera* camera) override;
    

    // Input processing
    void HandleInput();

    // Damage & State interaction
    void TakeDamage(int damage);
    void Heal(int amount);
    void Kill();
    void Respawn(float x, float y);
    void LockControls();
    bool IsAttacking() const;
    bool IsInvulnerable() const;
    // Center-based overlap against the active attack, including bullets.
    bool IntersectsAttack(float cx, float cy, float w, float h);
    // Changes once per punch, slash, shot, or kick, so one swing hits once.
    int GetAttackSerial() const { return m_attackSerial; }
    int AttackDamage() const;

    DecadeState GetCurrentState() const { return m_state; }
    int GetHealth() const { return m_health; }
};
