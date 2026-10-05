#pragma once
#include <windows.h>
#include <string>
#include <unordered_map>
#include "GameObject.h"
#include "../Framework/Animation.h"
#include "../Framework/TextureManager.h"
#include "../Framework/Constants.h"

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

    // Dimension Kick tunnel cards
    struct DimensionCard {
        float x, y;
        bool isShattered;
    };
    std::vector<DimensionCard> m_dimensionCards;
    Sprite* m_cardSprite;

    void SetState(DecadeState newState);

public:
    Decade(float startX, float startY);
    virtual ~Decade();

    void InitAnimations(LPDIRECT3DDEVICE9 d3ddev);
    void Update(float dt, const TileMap* tileMap);
    virtual void Update(float dt) override { Update(dt, nullptr); }
    void Render(LPD3DXSPRITE spriteHandler, const Camera* camera);
    virtual void Render(LPD3DXSPRITE spriteHandler) override { Render(spriteHandler, nullptr); }

    // Input processing
    void HandleInput();

    // Damage & State interaction
    void TakeDamage(int damage);
    bool IsAttacking() const;
    bool IsInvulnerable() const;

    DecadeState GetCurrentState() const { return m_state; }
    int GetHealth() const { return m_health; }
};
