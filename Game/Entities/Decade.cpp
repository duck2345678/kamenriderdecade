#include "Game/Entities/Decade.h"
#include "Game/Entities/DecadeSprites.h"
#include "Engine/Graphics/Camera.h"
#include "Game/World/TileMap.h"
#include "Engine/Graphics/TextureManager.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr float kKickInsert = 0.75f; // frames 0-4: card into the belt, then the kick
constexpr float kKickWindup = 0.16f;
constexpr float kKickRise = 0.26f;
constexpr float kKickFly = 0.62f;
constexpr float kKickApex = 128.0f;     // how high Decade rises before the dive
constexpr float kKickRiseFwd = 30.0f;   // forward drift while rising
constexpr float kKickDiveFwd = 230.0f;  // horizontal distance covered by the dive
constexpr float kKickDiveEnd = 10.0f;   // dive hands over to physics this far above origin
constexpr int kKickCards = 6;
constexpr float kSpriteScale = 0.25f;
constexpr float kCardBaseScale = 0.34f;
constexpr float kCardShatterTime = 0.22f;
}

Decade::Decade(float startX, float startY)
    : GameObject(ObjectType::Player, startX, startY, 24.0f, 48.0f),
      m_state(DecadeState::Idle),
      m_flipX(false),
      m_isGrounded(true),
      m_health(100),
      m_maxHealth(100),
      m_cardSprite(nullptr),
      m_kickTime(0.0f), m_kickOriginX(0.0f), m_kickOriginY(0.0f),
      m_kickDir(1.0f), m_kickAngle(0.0f), m_kickScripted(false),
      m_bulletSprite(nullptr),
      m_previousPunchDown(false), m_previousSlashDown(false),
      m_previousShootDown(false), m_previousKickDown(false),
      m_controlLocked(false), m_attackSerial(0) {
    m_type = ObjectType::Player;
}

Decade::~Decade() {
    for (auto& pair : m_animations) delete pair.second;
    m_animations.clear();
    for (auto& pair : m_sprites) delete pair.second;
    m_sprites.clear();

    if (m_cardSprite) { delete m_cardSprite; m_cardSprite = nullptr; }
    if (m_bulletSprite) { delete m_bulletSprite; m_bulletSprite = nullptr; }
}

void Decade::InitAnimations(LPDIRECT3DDEVICE9 d3ddev) {
    auto tm = TextureManager::GetInstance();
    auto configs = GetDecadeAnimationConfigs();

    for (const auto& config : configs) {
        LPDIRECT3DTEXTURE9 tex = tm->LoadTexture(d3ddev, config.texturePath);
        if (!tex) continue;

        Sprite* sprite = new Sprite(tex);
        m_sprites[config.name] = sprite;

        Animation* anim = new Animation(sprite, config.isLoop);
        for (const auto& frame : config.frames) {
            anim->AddFrame(frame.rect, frame.duration);
        }
        m_animations[config.name] = anim;
    }

    LPDIRECT3DTEXTURE9 cardTex = tm->LoadTexture(d3ddev, L"Assets/Textures/vfx_decade_card.png");
    if (cardTex) m_cardSprite = new Sprite(cardTex, 1376, 414);

    LPDIRECT3DTEXTURE9 bulletTex = tm->LoadTexture(d3ddev, L"Assets/Textures/vfx_bullet_decade.png");
    if (bulletTex) m_bulletSprite = new Sprite(bulletTex, 152, 66);
}

void Decade::SetState(DecadeState newState) {
    if (m_state == newState) return;
    m_state = newState;
    if (newState == DecadeState::AttackPunch || newState == DecadeState::AttackSlash ||
        newState == DecadeState::AttackShoot || newState == DecadeState::DimensionKick) {
        m_attackSerial++;
    }
    
    std::string currentAnim = "IDLE";
    switch (m_state) {
        case DecadeState::Idle:          currentAnim = "IDLE"; break;
        case DecadeState::Walk:          currentAnim = "WALK"; break;
        case DecadeState::Run:           currentAnim = "WALK"; break;
        case DecadeState::Jump:          currentAnim = "JUMP"; break;
        case DecadeState::Fall:          currentAnim = "JUMP"; break;
        case DecadeState::Block:         currentAnim = "BLOCK"; break;
        case DecadeState::AttackPunch:   currentAnim = "ATTACK_J"; break;
        case DecadeState::AttackSlash:   currentAnim = "ATTACK_K"; break;
        case DecadeState::AttackShoot:   currentAnim = "ATTACK_L"; break;
        case DecadeState::DimensionKick: currentAnim = "DIMENSION_KICK"; break;
        case DecadeState::Hurt:          currentAnim = "HURT"; break;
        case DecadeState::Defeat:        currentAnim = "DEFEAT"; break;
    }
    
    if (m_animations.find(currentAnim) != m_animations.end()) {
        m_animations[currentAnim]->Reset();
    }

}

std::string Decade::CurrentAnimName() const {
    switch (m_state) {
        case DecadeState::Idle:          return "IDLE";
        case DecadeState::Walk:          return "WALK";
        case DecadeState::Run:           return "WALK";
        case DecadeState::Jump:          return "JUMP";
        case DecadeState::Fall:          return "JUMP";
        case DecadeState::Block:         return "BLOCK";
        case DecadeState::AttackPunch:   return "ATTACK_J";
        case DecadeState::AttackSlash:   return "ATTACK_K";
        case DecadeState::AttackShoot:   return "ATTACK_L";
        case DecadeState::DimensionKick: return "DIMENSION_KICK";
        case DecadeState::Hurt:          return "HURT";
        case DecadeState::Defeat:        return "DEFEAT";
    }
    return "IDLE";
}

float Decade::GetAnimScale(const std::string& anim) const {
    // Sheets were cut from different source sizes. Normalise so a standing
    // Decade is the same height (~70 px) in every animation.
    // Standing frame heights: IDLE ~280, ATTACK_J ~180, DIMENSION_KICK ~219.
    if (anim == "ATTACK_J") return kSpriteScale * (280.0f / 180.0f);
    if (anim == "DIMENSION_KICK") return kSpriteScale * (280.0f / 219.0f);
    return kSpriteScale;
}

void Decade::GetSpriteAnchor(const std::string& anim, int frame, float frameW, float frameH, float& anchorX, float& anchorY) const {
    anchorX = frameW * 0.5f;
    anchorY = frameH;

    if (anim == "ATTACK_J") {
        static const float kFeet[] = { 46.0f, 61.0f, 63.0f, 64.0f, 60.0f, 69.0f, 40.0f, 55.0f, 63.0f };
        if (frame >= 0 && frame < 9) anchorX = kFeet[frame];
    } else if (anim == "ATTACK_K") {
        static const float kFeet[] = { 102.0f, 107.0f, 109.0f, 113.0f, 106.0f };
        if (frame >= 0 && frame < 5) anchorX = kFeet[frame];
    } else if (anim == "ATTACK_L") {
        anchorX = 104.0f;
    } else if (anim == "DIMENSION_KICK") {
        static const float kFeet[] = { 43.0f, 42.0f, 42.0f, 46.0f, 40.0f, 17.0f, 48.0f, 95.0f, 126.0f, 94.0f };
        if (frame >= 0 && frame < 10) anchorX = kFeet[frame];
        if (frame == 7 || frame == 8) {
            // Airborne frames rotate around the body center.
            anchorX = frameW * 0.5f;
            anchorY = frameH * 0.5f;
        }
    }
}

void Decade::SpawnBullet() {
    const float dir = m_flipX ? -1.0f : 1.0f;
    const float muzzleX = m_x + dir * 48.0f;
    const float muzzleY = m_y - 20.0f;
    m_bullets.push_back({ muzzleX, muzzleY, dir * 560.0f, 0.0f, true });
}

void Decade::BeginDimensionKick() {
    m_kickDir = m_flipX ? -1.0f : 1.0f;
    m_kickTime = 0.0f;
    m_kickOriginX = m_x;
    m_kickOriginY = m_y;
    m_kickScripted = true;
    m_isGrounded = false;
    m_vx = 0.0f;
    m_vy = 0.0f;
    SetState(DecadeState::DimensionKick);

    // Dive line: from the apex down-forward to just above the take-off height.
    const float diveDx = kKickDiveFwd;
    const float diveDy = kKickApex - kKickDiveEnd;
    m_kickAngle = std::atan2(diveDy, diveDx);

    // Cards sit on the dive line, evenly spaced, so Decade punches through
    // each one in sequence as he descends.
    m_dimensionCards.clear();
    const float apexX = m_kickOriginX + m_kickDir * kKickRiseFwd;
    const float apexY = m_kickOriginY - kKickApex;
    for (int i = 0; i < kKickCards; ++i) {
        const float t = 0.16f + i * 0.14f;
        DimensionCard card;
        card.x = apexX + m_kickDir * diveDx * t;
        card.y = apexY + diveDy * t;
        card.style = i % 3;
        card.depth = (kKickCards > 1) ? (float)i / (float)(kKickCards - 1) : 0.0f;
        card.shatter = -1.0f;
        m_dimensionCards.push_back(card);
    }
}

void Decade::UpdateDimensionKick(float dt, const TileMap* tileMap) {
    m_kickTime += dt;
    const float insertEnd = kKickInsert;
    const float windupEnd = insertEnd + kKickWindup;
    const float riseEnd = windupEnd + kKickRise;
    const float flyEnd = riseEnd + kKickFly;
    const float apexX = m_kickOriginX + m_kickDir * kKickRiseFwd;
    const float apexY = m_kickOriginY - kKickApex;
    const float diveDx = kKickDiveFwd;
    const float diveDy = kKickApex - kKickDiveEnd;

    if (m_kickTime < insertEnd) {
        // Stand and slot the rider card into the belt before leaving the ground.
        m_x = m_kickOriginX;
        m_y = m_kickOriginY;
        m_vx = 0.0f;
        m_vy = 0.0f;
        m_isGrounded = true;
    } else if (m_kickTime < windupEnd) {
        const float u = (m_kickTime - insertEnd) / kKickWindup;
        m_x = m_kickOriginX;
        m_y = m_kickOriginY + 3.0f * std::sin(u * 3.14159f);
        m_vx = 0.0f;
        m_vy = 0.0f;
    } else if (m_kickTime < riseEnd) {
        // Leap up to the apex (ease-out).
        float u = (m_kickTime - windupEnd) / kKickRise;
        if (u < 0.0f) u = 0.0f;
        if (u > 1.0f) u = 1.0f;
        const float e = 1.0f - (1.0f - u) * (1.0f - u);
        m_x = m_kickOriginX + m_kickDir * kKickRiseFwd * e;
        m_y = m_kickOriginY - kKickApex * e;
        m_vx = 0.0f;
        m_vy = 0.0f;
    } else if (m_kickTime < flyEnd) {
        // Dive down-forward along the card line (ease-in so he accelerates).
        float u = (m_kickTime - riseEnd) / kKickFly;
        if (u > 1.0f) u = 1.0f;
        const float e = u * (0.55f + 0.45f * u);
        const float nextX = apexX + m_kickDir * diveDx * e;
        const float nextY = apexY + diveDy * e;

        bool blocked = false;
        if (tileMap) {
            float corrected = nextX;
            const float stepX = nextX - m_x;
            if (stepX != 0.0f && tileMap->CheckHorizontalCollision(nextX, nextY, m_width, m_height, stepX, corrected)) {
                m_x = corrected;
                m_y = nextY;
                blocked = true;
            }
            float groundY = 0.0f;
            if (!blocked && tileMap->CheckGroundCollision(nextX, nextY, m_width, m_height, groundY) && groundY <= nextY) {
                m_x = nextX;
                m_y = groundY;
                m_vx = 0.0f;
                m_vy = 0.0f;
                m_isGrounded = true;
                m_kickScripted = false;
                SetState(DecadeState::Idle);
                blocked = true;
            }
        }
        if (!blocked) {
            m_x = nextX;
            m_y = nextY;
            m_vx = 0.0f;
            m_vy = 0.0f;
        } else if (m_kickScripted) {
            // Hit a wall mid-dive: drop out of the script and fall.
            m_kickScripted = false;
            m_vx = 0.0f;
            m_vy = 120.0f;
        }
    } else {
        // Hand over to physics, keeping the dive direction.
        const float speed = 360.0f;
        m_kickScripted = false;
        m_vx = m_kickDir * speed * std::cos(m_kickAngle);
        m_vy = speed * std::sin(m_kickAngle);
    }

    for (auto& card : m_dimensionCards) {
        if (card.shatter >= 0.0f) continue;
        const float dx = m_x - card.x;
        const float dy = m_y - card.y;
        if (dx * dx + dy * dy < 30.0f * 30.0f) {
            card.shatter = 0.0f;
        }
    }

    if (!m_kickScripted) {
        for (auto& card : m_dimensionCards) {
            if (card.shatter < 0.0f) card.shatter = 0.0f;
        }
    }
}

void Decade::HandleInput() {
    if (m_controlLocked) return;
    if (m_state == DecadeState::Defeat || m_state == DecadeState::Hurt) return;
    if (IsAttacking()) return;

    bool keyLeft = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;
    bool keyRight = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;
    bool keyDown = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;
    bool keyJump = ((GetAsyncKeyState(VK_SPACE) & 0x8000) != 0) || ((GetAsyncKeyState(VK_UP) & 0x8000) != 0);

    bool keyPunch = (GetAsyncKeyState('J') & 0x8000) != 0;
    bool keySlash = (GetAsyncKeyState('K') & 0x8000) != 0;
    bool keyShoot = (GetAsyncKeyState('L') & 0x8000) != 0;
    bool keyKick  = (GetAsyncKeyState('O') & 0x8000) != 0;

    bool punchPressed = keyPunch && !m_previousPunchDown;
    bool slashPressed = keySlash && !m_previousSlashDown;
    bool shootPressed = keyShoot && !m_previousShootDown;
    bool kickPressed = keyKick && !m_previousKickDown;

    m_previousPunchDown = keyPunch;
    m_previousSlashDown = keySlash;
    m_previousShootDown = keyShoot;
    m_previousKickDown = keyKick;

    if (kickPressed) {
        BeginDimensionKick();
        return;
    }
    if (shootPressed) {
        SetState(DecadeState::AttackShoot);
        m_vx = 0.0f;
        m_vy = 0.0f;
        SpawnBullet();
        return;
    }
    if (slashPressed) { SetState(DecadeState::AttackSlash); m_vx = 0.0f; return; }
    if (punchPressed) { SetState(DecadeState::AttackPunch); m_vx = 0.0f; return; }
    if (keyDown) { SetState(DecadeState::Block); m_vx = 0.0f; return; }

    if (keyJump && m_isGrounded) {
        SetState(DecadeState::Jump);
        m_vy = -400.0f;
        m_isGrounded = false;
    }

    if (keyLeft) {
        m_vx = -200.0f;
        m_flipX = true;
        if (m_isGrounded && m_state != DecadeState::Jump) SetState(DecadeState::Walk);
    } else if (keyRight) {
        m_vx = 200.0f;
        m_flipX = false;
        if (m_isGrounded && m_state != DecadeState::Jump) SetState(DecadeState::Walk);
    } else {
        m_vx = 0.0f;
        if (m_isGrounded && m_state != DecadeState::Jump && m_state != DecadeState::Fall) {
            SetState(DecadeState::Idle);
        }
    }
}

void Decade::Update(float dt, const TileMap* tileMap) {
    HandleInput();

    const bool groundedAttack =
        m_state == DecadeState::AttackPunch ||
        m_state == DecadeState::AttackSlash ||
        m_state == DecadeState::AttackShoot;

    if (m_state == DecadeState::DimensionKick && m_kickScripted) {
        UpdateDimensionKick(dt, tileMap);
    } else if (groundedAttack && m_isGrounded) {
        m_vx = 0.0f;
        m_vy = 0.0f;
        if (tileMap) {
            float groundY = 0.0f;
            if (tileMap->CheckGroundCollision(m_x, m_y, m_width, m_height, groundY)) {
                m_y = groundY;
                m_isGrounded = true;
            } else {
                m_isGrounded = false;
            }
        }
    } else {
        if (!m_isGrounded) {
            m_vy += GRAVITY * dt;
            if (m_vy > MAX_FALL_SPEED) m_vy = MAX_FALL_SPEED;

            if (!IsAttacking()) {
                SetState(m_vy < 0 ? DecadeState::Jump : DecadeState::Fall);
            }
        }

        float newX = m_x + m_vx * dt;
        float correctedX = newX;
        if (tileMap && tileMap->CheckHorizontalCollision(newX, m_y, m_width, m_height, m_vx, correctedX)) {
            m_x = correctedX;
            m_vx = 0.0f;
        } else {
            m_x = newX;
        }

        m_y += m_vy * dt;

        if (tileMap) {
            const bool wasGrounded = m_isGrounded;
            float groundY = 0.0f;
            if (m_vy >= 0 && tileMap->CheckGroundCollision(m_x, m_y, m_width, m_height, groundY)) {
                m_y = groundY;
                m_vy = 0.0f;
                m_isGrounded = true;

                if (!wasGrounded && m_state == DecadeState::Fall) {
                    SetState(m_vx != 0.0f ? DecadeState::Walk : DecadeState::Idle);
                }
                if (!wasGrounded && m_state == DecadeState::DimensionKick && !m_kickScripted) {
                    SetState(m_vx != 0.0f ? DecadeState::Walk : DecadeState::Idle);
                }
            } else {
                m_isGrounded = false;
            }
        }
    }

    for (auto& card : m_dimensionCards) {
        if (card.shatter >= 0.0f) card.shatter += dt;
    }
    m_dimensionCards.erase(
        std::remove_if(m_dimensionCards.begin(), m_dimensionCards.end(),
            [](const DimensionCard& card) { return card.shatter > kCardShatterTime; }),
        m_dimensionCards.end());

    for (auto& b : m_bullets) {
        if (b.active) {
            b.x += b.vx * dt;
            b.life += dt;
            if (b.life > 1.2f) b.active = false;
        }
    }
    m_bullets.erase(
        std::remove_if(m_bullets.begin(), m_bullets.end(),
            [](const Bullet& b) { return !b.active; }),
        m_bullets.end());

    const std::string currentAnim = CurrentAnimName();
    auto animIt = m_animations.find(currentAnim);
    if (animIt != m_animations.end()) {
        Animation* anim = animIt->second;
        if (m_state == DecadeState::DimensionKick) {
            int frame = 9;
            const float insertEnd = kKickInsert;
            const float windupEnd = insertEnd + kKickWindup;
            const float riseEnd = windupEnd + kKickRise;
            if (m_kickScripted) {
                if (m_kickTime < insertEnd) {
                    frame = (int)(m_kickTime / (kKickInsert / 5.0f));
                    if (frame > 4) frame = 4;
                } else if (m_kickTime < windupEnd) frame = 5;
                else if (m_kickTime < riseEnd) frame = 6;
                else if (m_kickTime < riseEnd + 0.07f) frame = 7;
                else frame = 8;
            } else if (m_vy > 0.0f && !m_isGrounded) {
                frame = 8; // still diving under physics
            }
            anim->SetFrame(frame);
        } else {
            anim->Update(dt);
            if (anim->IsFinished() && (IsAttacking() || m_state == DecadeState::Hurt)) {
                SetState(m_isGrounded ? DecadeState::Idle : DecadeState::Fall);
            }
        }
    }
}

void Decade::Render(LPD3DXSPRITE spriteHandler, const Camera* camera) {
    if (!m_isAlive) return;

    if (m_cardSprite) {
        static const RECT kCardFrames[] = {
            { 59, 55, 151, 322 },
            { 311, 55, 404, 321 },
            { 608, 55, 700, 321 }
        };
        static const RECT kCardEdge = { 1138, 73, 1176, 304 };

        // Pseudo-3D: each card is a plane the rider punches through, so it is
        // rotated to sit perpendicular to the dive line, squashed horizontally
        // (seen at an angle) and sheared for lean. Cards farther down the
        // tunnel get smaller and dimmer for depth.
        const float tilt = m_kickDir * m_kickAngle;
        const float shear = -0.28f * m_kickDir;
        const float squash = 0.62f;

        // Draw far cards first so nearer ones overlap them.
        const float cardsAppear = kKickInsert + kKickWindup;
        for (size_t n = m_dimensionCards.size(); n-- > 0;) {
            const DimensionCard& card = m_dimensionCards[n];
            if (card.shatter < 0.0f && m_kickTime < cardsAppear) continue;
            float cX = card.x;
            float cY = card.y;
            if (camera) camera->WorldToScreen(card.x, card.y, cX, cY);

            const float depthScale = kCardBaseScale * (1.0f - 0.32f * card.depth);
            const int depthAlpha = 250 - (int)(70.0f * card.depth);

            if (card.shatter < 0.0f) {
                const RECT& src = kCardFrames[card.style % 3];
                m_cardSprite->DrawEx(spriteHandler, cX, cY, &src,
                    depthScale * squash, depthScale, shear, tilt,
                    D3DCOLOR_ARGB(depthAlpha, 255, 255, 255));
            } else {
                const float u = card.shatter / kCardShatterTime;
                int alpha = (int)(depthAlpha * (1.0f - u));
                if (alpha < 0) alpha = 0;
                const float burst = depthScale * (1.0f + 0.6f * u);
                m_cardSprite->DrawEx(spriteHandler, cX, cY, &kCardEdge,
                    burst * 0.9f, burst, shear * 0.5f, tilt,
                    D3DCOLOR_ARGB(alpha, 255, 255, 255));
            }
        }
    }

    const std::string currentAnim = CurrentAnimName();
    auto animIt = m_animations.find(currentAnim);
    if (animIt != m_animations.end()) {
        Animation* anim = animIt->second;
        RECT srcRect = anim->GetCurrentFrameRect();
        const float frameW = (float)(srcRect.right - srcRect.left);
        const float frameH = (float)(srcRect.bottom - srcRect.top);
        const int frameIndex = anim->GetCurrentFrameIndex();

        float anchorX = frameW * 0.5f;
        float anchorY = frameH;
        GetSpriteAnchor(currentAnim, frameIndex, frameW, frameH, anchorX, anchorY);

        const bool flyingKick = (m_state == DecadeState::DimensionKick && (frameIndex == 7 || frameIndex == 8));
        const float scale = GetAnimScale(currentAnim);
        const float worldX = m_x;
        const float worldY = flyingKick ? m_y : (m_y + m_height * 0.5f);

        // Align the body with the dive so the kick drives forward through the cards.
        float rotation = 0.0f;
        if (flyingKick && frameIndex == 8) rotation = m_kickDir * m_kickAngle;

        const float ox = (anchorX - frameW * 0.5f) * scale;
        const float oy = (anchorY - frameH * 0.5f) * scale;
        float centerX = m_flipX ? (worldX + ox) : (worldX - ox);
        float centerY = worldY - oy;
        if (camera) camera->WorldToScreen(centerX, centerY, centerX, centerY);

        anim->Render(spriteHandler, centerX, centerY, m_flipX, scale, D3DCOLOR_ARGB(255, 255, 255, 255), rotation);
    }

    if (m_bulletSprite) {
        const RECT bulletSrc = { 29, 17, 133, 43 };
        for (const auto& b : m_bullets) {
            float renderX = b.x;
            float renderY = b.y;
            if (camera) camera->WorldToScreen(b.x, b.y, renderX, renderY);
            m_bulletSprite->Draw(spriteHandler, renderX, renderY, &bulletSrc, b.vx < 0.0f, 0.36f, D3DCOLOR_ARGB(255, 255, 255, 255));
        }
    }
}

bool Decade::IsAttacking() const {
    return (m_state == DecadeState::AttackPunch ||
            m_state == DecadeState::AttackSlash ||
            m_state == DecadeState::AttackShoot ||
            m_state == DecadeState::DimensionKick);
}

bool Decade::IsInvulnerable() const {
    if (m_state == DecadeState::Block) return true;
    if (m_state == DecadeState::DimensionKick) {
        const float diveStart = kKickInsert + kKickWindup + kKickRise;
        return m_kickTime >= diveStart;
    }
    return false;
}

int Decade::AttackDamage() const {
    if (m_state == DecadeState::AttackSlash || m_state == DecadeState::DimensionKick) return 2;
    return 1;
}

void Decade::Heal(int amount) {
    if (m_state == DecadeState::Defeat || amount <= 0) return;
    m_health += amount;
    if (m_health > m_maxHealth) m_health = m_maxHealth;
}

void Decade::Kill() {
    if (m_state == DecadeState::Defeat) return;
    m_health = 0;
    m_kickScripted = false;
    m_vx = 0.0f;
    SetState(DecadeState::Defeat);
}

void Decade::Respawn(float x, float y) {
    m_x = x;
    m_y = y;
    m_vx = 0.0f;
    m_vy = 0.0f;
    m_isGrounded = true;
    m_health = m_maxHealth;
    m_controlLocked = false;
    m_kickScripted = false;
    m_dimensionCards.clear();
    m_bullets.clear();
    m_state = DecadeState::Fall;
    SetState(DecadeState::Idle);
}

void Decade::LockControls() {
    m_controlLocked = true;
    m_vx = 0.0f;
}

bool Decade::IntersectsAttack(float cx, float cy, float w, float h) {
    auto overlaps = [](float ax, float ay, float aw, float ah, float bx, float by, float bw, float bh) {
        return std::abs(ax - bx) * 2.0f < (aw + bw) && std::abs(ay - by) * 2.0f < (ah + bh);
    };

    bool melee = IsAttacking() && m_state != DecadeState::AttackShoot;
    if (m_state == DecadeState::DimensionKick) {
        const float diveStart = kKickInsert + kKickWindup + kKickRise;
        melee = m_kickTime >= diveStart;
    }
    if (melee) {
        const float dir = m_flipX ? -1.0f : 1.0f;
        const float ax = m_x + dir * 20.0f;
        if (overlaps(ax, m_y, 42.0f, 36.0f, cx, cy, w, h)) return true;
    }

    for (auto& bullet : m_bullets) {
        if (!bullet.active) continue;
        if (overlaps(bullet.x, bullet.y, 18.0f, 10.0f, cx, cy, w, h)) {
            bullet.active = false;
            return true;
        }
    }
    return false;
}

void Decade::TakeDamage(int damage) {
    if (IsInvulnerable() || m_state == DecadeState::Defeat || m_state == DecadeState::Hurt) return;
    if (m_state == DecadeState::Block) damage = (int)(damage * 0.2f);

    m_health -= damage;
    if (m_health <= 0) {
        m_health = 0;
        SetState(DecadeState::Defeat);
    } else {
        SetState(DecadeState::Hurt);
    }
}
