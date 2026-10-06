#include "Game/Entities/LevelActors.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Graphics/TextureManager.h"
#include <cmath>

namespace {
constexpr float kShockerScale = 0.15f;
constexpr int kShockerHp = 2;
constexpr float kSightX = 280.0f;
constexpr float kSightY = 46.0f;
constexpr float kAttackX = 52.0f;
constexpr float kAttackY = 40.0f;
constexpr float kPatrolSpeed = 56.0f;
constexpr float kChaseSpeed = 118.0f;

Animation* MakeAnim(Sprite* sprite, bool loop, const RECT* frames, const float* durations, int count) {
    Animation* anim = new Animation(sprite, loop);
    for (int i = 0; i < count; ++i) anim->AddFrame(frames[i], durations[i]);
    return anim;
}
}

Shocker::Shocker(float x, float surfaceTop, float left, float right)
    : GameObject(ObjectType::EnemyGrunt, x, surfaceTop - 22.0f, 22.0f, 44.0f),
      m_walk(nullptr), m_attack(nullptr), m_hurt(nullptr), m_die(nullptr),
      m_walkSprite(nullptr), m_attackSprite(nullptr), m_hurtSprite(nullptr), m_dieSprite(nullptr),
      m_left(left), m_right(right), m_homeX(x), m_dir(1), m_hp(kShockerHp),
      m_lastHitSerial(0), m_hitFromX(x), m_swingConnected(false),
      m_state(State::Patrol), m_stateTime(0.0f) {
}

Shocker::~Shocker() {
    delete m_walk;
    delete m_attack;
    delete m_hurt;
    delete m_die;
    delete m_walkSprite;
    delete m_attackSprite;
    delete m_hurtSprite;
    delete m_dieSprite;
}

void Shocker::Init(LPDIRECT3DDEVICE9 device) {
    TextureManager* tm = TextureManager::GetInstance();

    LPDIRECT3DTEXTURE9 walkTex = tm->LoadTexture(device, L"Assets/Textures/shocker_walk.png");
    if (walkTex) {
        m_walkSprite = new Sprite(walkTex);
        const RECT frames[] = {
            { 34, 48, 236, 453 },
            { 297, 48, 506, 452 },
            { 605, 48, 784, 456 },
            { 882, 48, 1097, 456 },
            { 1177, 48, 1387, 455 },
            { 1459, 47, 1665, 452 }
        };
        const float durations[] = { 0.11f, 0.11f, 0.11f, 0.11f, 0.11f, 0.11f };
        m_walk = MakeAnim(m_walkSprite, true, frames, durations, 6);
    }

    LPDIRECT3DTEXTURE9 attackTex = tm->LoadTexture(device, L"Assets/Textures/shocker_attack.png");
    if (attackTex) {
        m_attackSprite = new Sprite(attackTex);
        const RECT frames[] = {
            { 102, 16, 415, 501 },
            { 470, 109, 915, 499 },
            { 979, 129, 1256, 519 }
        };
        const float durations[] = { 0.12f, 0.16f, 0.16f };
        m_attack = MakeAnim(m_attackSprite, false, frames, durations, 3);
    }

    LPDIRECT3DTEXTURE9 hurtTex = tm->LoadTexture(device, L"Assets/Textures/shocker_hurt.png");
    if (hurtTex) {
        m_hurtSprite = new Sprite(hurtTex);
        const RECT frames[] = {
            { 53, 31, 347, 440 },
            { 447, 45, 786, 451 }
        };
        const float durations[] = { 0.08f, 0.22f };
        m_hurt = MakeAnim(m_hurtSprite, false, frames, durations, 2);
    }

    LPDIRECT3DTEXTURE9 dieTex = tm->LoadTexture(device, L"Assets/Textures/shocker_die.png");
    if (dieTex) {
        m_dieSprite = new Sprite(dieTex);
        const RECT frames[] = {
            { 86, 33, 387, 423 },
            { 447, 118, 817, 422 },
            { 875, 325, 1239, 432 }
        };
        const float durations[] = { 0.10f, 0.16f, 0.50f };
        m_die = MakeAnim(m_dieSprite, false, frames, durations, 3);
    }
}

void Shocker::Reset() {
    m_x = m_homeX;
    m_vx = 0.0f;
    m_vy = 0.0f;
    m_dir = 1;
    m_hp = kShockerHp;
    m_lastHitSerial = 0;
    m_swingConnected = false;
    m_state = State::Patrol;
    m_stateTime = 0.0f;
    m_isAlive = true;
    if (m_walk) m_walk->Reset();
    if (m_attack) m_attack->Reset();
    if (m_hurt) m_hurt->Reset();
    if (m_die) m_die->Reset();
}

bool Shocker::HasFooting(const TileMap* tileMap, float x) const {
    if (x < m_left || x > m_right) return false;
    if (!tileMap) return true;
    const float feet = m_y + m_height * 0.5f;
    return tileMap->IsSolidAt(x, feet + 6.0f);
}

bool Shocker::Sees(float playerX, float playerY) const {
    const float dx = playerX - m_x;
    const float dy = playerY - m_y;
    return std::fabs(dx) <= kSightX && std::fabs(dy) <= kSightY;
}

void Shocker::BeginAttack(float playerX) {
    m_state = State::Attack;
    m_stateTime = 0.0f;
    m_swingConnected = false;
    m_vx = 0.0f;
    if (std::fabs(playerX - m_x) > 6.0f) m_dir = (playerX >= m_x) ? 1 : -1;
    if (m_attack) m_attack->Reset();
}

void Shocker::SnapToGround(const TileMap* tileMap, float dt) {
    if (!tileMap) return;
    m_vy += 980.0f * dt;
    if (m_vy > 600.0f) m_vy = 600.0f;
    m_y += m_vy * dt;
    float groundY = 0.0f;
    if (m_vy >= 0.0f && tileMap->CheckGroundCollision(m_x, m_y, m_width, m_height, groundY)) {
        m_y = groundY;
        m_vy = 0.0f;
    }
}

void Shocker::TakeHit(int damage, int attackSerial, float fromX) {
    if (!IsTargetable() || damage <= 0) return;
    if (attackSerial != 0 && attackSerial == m_lastHitSerial) return;
    m_lastHitSerial = attackSerial;
    m_hitFromX = fromX;
    m_hp -= damage;
    m_stateTime = 0.0f;
    m_vx = 0.0f;
    if (m_hp <= 0) {
        m_hp = 0;
        m_state = State::Die;
        if (m_die) m_die->Reset();
    } else {
        m_state = State::Hurt;
        m_dir = (fromX >= m_x) ? -1 : 1;
        if (m_hurt) m_hurt->Reset();
    }
}

bool Shocker::AttackHits(float px, float py, float pw, float ph) {
    if (m_state != State::Attack || m_swingConnected || !m_attack) return false;
    if (m_attack->GetCurrentFrameIndex() != 1) return false;
    const float ax = m_x + (float)m_dir * 36.0f;
    const float ay = m_y - 2.0f;
    const bool hit = std::fabs(ax - px) * 2.0f < (52.0f + pw) && std::fabs(ay - py) * 2.0f < (36.0f + ph);
    if (hit) m_swingConnected = true;
    return hit;
}

void Shocker::Update(float dt, const TileMap* tileMap, float playerX, float playerY) {
    if (!m_isAlive) return;
    m_stateTime += dt;

    if (m_state == State::Die) {
        if (m_die) m_die->Update(dt);
        if ((m_die && m_die->IsFinished()) || m_stateTime > 0.9f) m_isAlive = false;
        return;
    }

    if (m_state == State::Hurt) {
        const float away = (m_x < m_hitFromX) ? -1.0f : 1.0f;
        const float nextX = m_x + away * 80.0f * dt;
        if (HasFooting(tileMap, nextX)) m_x = nextX;
        if (m_hurt) m_hurt->Update(dt);
        SnapToGround(tileMap, dt);
        if ((m_hurt && m_hurt->IsFinished()) || m_stateTime > 0.4f) {
            m_state = Sees(playerX, playerY) ? State::Chase : State::Patrol;
            m_stateTime = 0.0f;
        }
        return;
    }

    if (m_state == State::Attack) {
        m_vx = 0.0f;
        if (m_attack) m_attack->Update(dt);
        SnapToGround(tileMap, dt);
        if ((m_attack && m_attack->IsFinished()) || m_stateTime > 0.55f) {
            m_state = Sees(playerX, playerY) ? State::Chase : State::Patrol;
            m_stateTime = 0.0f;
        }
        return;
    }

    const bool see = Sees(playerX, playerY);
    if (see) m_state = State::Chase;
    else m_state = State::Patrol;

    if (m_state == State::Chase &&
        std::fabs(playerX - m_x) <= kAttackX &&
        std::fabs(playerY - m_y) <= kAttackY) {
        BeginAttack(playerX);
        SnapToGround(tileMap, dt);
        return;
    }

    if (m_state == State::Chase && std::fabs(playerX - m_x) > 8.0f) {
        m_dir = (playerX >= m_x) ? 1 : -1;
    }

    const float speed = (m_state == State::Chase) ? kChaseSpeed : kPatrolSpeed;
    float nextX = m_x + m_dir * speed * dt;
    const float probe = m_x + m_dir * 14.0f;
    bool blocked = !HasFooting(tileMap, probe);
    if (!blocked && tileMap) {
        float corrected = nextX;
        if (tileMap->CheckHorizontalCollision(nextX, m_y, m_width, m_height, m_dir * speed, corrected)) {
            blocked = true;
            nextX = corrected;
        }
    }
    if (blocked) {
        if (m_state == State::Patrol) m_dir = -m_dir;
    } else {
        m_x = nextX;
    }

    if (m_walk) m_walk->Update(blocked && m_state == State::Chase ? 0.0f : dt);
    SnapToGround(tileMap, dt);
}

void Shocker::RenderAnim(LPD3DXSPRITE spriteHandler, const Camera* camera, Animation* anim, const float* feet, int feetCount) const {
    if (!anim || !camera) return;
    RECT frame = anim->GetCurrentFrameRect();
    const float frameW = (float)(frame.right - frame.left);
    const float frameH = (float)(frame.bottom - frame.top);
    const int index = anim->GetCurrentFrameIndex();
    float anchorX = frameW * 0.5f;
    if (index >= 0 && index < feetCount) anchorX = feet[index];

    const float feetY = m_y + m_height * 0.5f;
    const float ox = (anchorX - frameW * 0.5f) * kShockerScale;
    const float oy = (frameH - frameH * 0.5f) * kShockerScale;
    const bool flip = m_dir < 0;
    float x = flip ? (m_x + ox) : (m_x - ox);
    float y = feetY - oy;
    camera->WorldToScreen(x, y, x, y);
    anim->Render(spriteHandler, x, y, flip, kShockerScale);
}

void Shocker::Render(LPD3DXSPRITE spriteHandler, const Camera* camera) {
    if (!m_isAlive) return;

    if (m_state == State::Die) {
        static const float kFeet[] = { 54.0f, 195.0f, 162.0f };
        RenderAnim(spriteHandler, camera, m_die, kFeet, 3);
        return;
    }
    if (m_state == State::Hurt) {
        static const float kFeet[] = { 51.0f, 114.0f };
        RenderAnim(spriteHandler, camera, m_hurt, kFeet, 2);
        return;
    }
    if (m_state == State::Attack) {
        static const float kFeet[] = { 24.0f, 24.0f, 27.0f };
        RenderAnim(spriteHandler, camera, m_attack, kFeet, 3);
        return;
    }

    static const float kFeet[] = { 36.0f, 189.0f, 127.0f, 180.0f, 175.0f, 39.0f };
    RenderAnim(spriteHandler, camera, m_walk, kFeet, 6);
}

CardPickup::CardPickup(float x, float y)
    : GameObject(ObjectType::CardItem, x, y, 18.0f, 28.0f),
      m_sprite(nullptr), m_baseY(y), m_time(0.0f), m_taken(false) {
}

CardPickup::~CardPickup() {
    delete m_sprite;
}

void CardPickup::Init(LPDIRECT3DDEVICE9 device) {
    LPDIRECT3DTEXTURE9 tex = TextureManager::GetInstance()->LoadTexture(device, L"Assets/Textures/vfx_decade_card.png");
    if (tex) m_sprite = new Sprite(tex);
}

void CardPickup::Reset() {
    m_taken = false;
    m_y = m_baseY;
    m_time = 0.0f;
}

void CardPickup::Update(float dt) {
    if (m_taken) return;
    m_time += dt;
    m_y = m_baseY + std::sin(m_time * 3.0f) * 4.0f;
}

void CardPickup::Render(LPD3DXSPRITE spriteHandler, const Camera* camera) {
    if (m_taken || !m_sprite || !camera) return;
    static const RECT src = { 59, 55, 151, 322 };
    float x = m_x;
    float y = m_y;
    camera->WorldToScreen(x, y, x, y);
    m_sprite->Draw(spriteHandler, x, y, &src, false, 0.15f, D3DCOLOR_ARGB(255, 255, 255, 255));
}
