#include "Decade.h"
#include "DecadeSprites.h"

Decade::Decade(float startX, float startY)
    : GameObject(startX, startY, 40.0f, 70.0f),
      m_state(DecadeState::Idle),
      m_flipX(false),
      m_isGrounded(true),
      m_health(100),
      m_maxHealth(100),
      m_lastLeftTapTime(0.0f),
      m_lastRightTapTime(0.0f),
      m_cardSprite(nullptr) {
    m_type = ObjectType::Player;
}

Decade::~Decade() {
    for (auto& pair : m_animations) {
        delete pair.second;
    }
    m_animations.clear();

    for (auto& pair : m_sprites) {
        delete pair.second;
    }
    m_sprites.clear();

    if (m_cardSprite) {
        delete m_cardSprite;
        m_cardSprite = nullptr;
    }
}

void Decade::InitAnimations(LPDIRECT3DDEVICE9 d3ddev) {
    TextureManager* tm = TextureManager::GetInstance();
    auto configs = GetDecadeAnimationConfigs();

    for (const auto& cfg : configs) {
        LPDIRECT3DTEXTURE9 tex = tm->LoadTexture(d3ddev, cfg.texturePath);
        if (!tex) continue;

        Sprite* sprite = new Sprite(tex);
        m_sprites[cfg.name] = sprite;

        Animation* anim = new Animation(sprite, cfg.isLoop);
        for (const auto& fr : cfg.frames) {
            anim->AddFrame(fr.rect, fr.duration);
        }
        m_animations[cfg.name] = anim;
    }

    // Load dimension card sprite for Final Attack Ride tunnel
    LPDIRECT3DTEXTURE9 cardTex = tm->LoadTexture(d3ddev, L"Assets/Textures/vfx_decade_card.png");
    if (cardTex) {
        m_cardSprite = new Sprite(cardTex, 92, 138);
    }
}

void Decade::SetState(DecadeState newState) {
    if (m_state == newState) return;
    m_state = newState;

    std::string animName = "IDLE";
    switch (m_state) {
        case DecadeState::Idle:          animName = "IDLE"; break;
        case DecadeState::Walk:          animName = "WALK"; break;
        case DecadeState::Run:           animName = "WALK"; break; // Walk cycle at higher speed
        case DecadeState::Jump:          animName = "JUMP"; break;
        case DecadeState::Fall:          animName = "JUMP"; break;
        case DecadeState::Block:         animName = "BLOCK"; break;
        case DecadeState::AttackPunch:   animName = "ATTACK_J"; break;
        case DecadeState::AttackSlash:   animName = "ATTACK_K"; break;
        case DecadeState::AttackShoot:   animName = "ATTACK_L"; break;
        case DecadeState::DimensionKick: animName = "DIMENSION_KICK"; break;
        case DecadeState::Hurt:          animName = "HURT"; break;
        case DecadeState::Defeat:        animName = "DEFEAT"; break;
    }

    if (m_animations.find(animName) != m_animations.end()) {
        m_animations[animName]->Reset();
    }

    // Dimension Kick card setup
    if (m_state == DecadeState::DimensionKick) {
        m_dimensionCards.clear();
        float dir = m_flipX ? -1.0f : 1.0f;
        for (int i = 1; i <= 4; ++i) {
            m_dimensionCards.push_back({ m_x + dir * (i * 60.0f), m_y + (i * 20.0f) - 40.0f, false });
        }
    }
}

void Decade::HandleInput() {
    if (m_state == DecadeState::Defeat || m_state == DecadeState::Hurt) return;

    // Do not interrupt locked attack animations until finished
    if (IsAttacking()) return;

    bool keyLeft = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;
    bool keyRight = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;
    bool keyDown = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;
    bool keyJump = ((GetAsyncKeyState(VK_SPACE) & 0x8000) != 0) || ((GetAsyncKeyState(VK_UP) & 0x8000) != 0);

    bool keyPunch = (GetAsyncKeyState('J') & 0x8000) != 0;
    bool keySlash = (GetAsyncKeyState('K') & 0x8000) != 0;
    bool keyShoot = (GetAsyncKeyState('L') & 0x8000) != 0;
    bool keyKick  = (GetAsyncKeyState('O') & 0x8000) != 0;

    // Attack triggers (Highest priority)
    if (keyKick) {
        SetState(DecadeState::DimensionKick);
        m_vx = m_flipX ? -280.0f : 280.0f;
        m_vy = -200.0f; // slight hop up then plunge
        return;
    }
    if (keyPunch) {
        SetState(DecadeState::AttackPunch);
        m_vx = 0.0f;
        return;
    }
    if (keySlash) {
        SetState(DecadeState::AttackSlash);
        m_vx = 0.0f;
        return;
    }
    if (keyShoot) {
        SetState(DecadeState::AttackShoot);
        m_vx = 0.0f;
        return;
    }

    // Defensive Block / Crouch
    if (keyDown && m_isGrounded) {
        SetState(DecadeState::Block);
        m_vx = 0.0f;
        return;
    }

    // Jump
    if (keyJump && m_isGrounded) {
        m_vy = -480.0f;
        m_isGrounded = false;
        SetState(DecadeState::Jump);
        return;
    }

    // Movement
    if (keyLeft) {
        m_flipX = true;
        m_vx = -160.0f;
        if (m_isGrounded) SetState(DecadeState::Walk);
    } else if (keyRight) {
        m_flipX = false;
        m_vx = 160.0f;
        if (m_isGrounded) SetState(DecadeState::Walk);
    } else {
        m_vx = 0.0f;
        if (m_isGrounded) SetState(DecadeState::Idle);
    }
}

void Decade::Update(float dt) {
    HandleInput();

    // Gravity
    if (!m_isGrounded) {
        m_vy += GRAVITY * dt;
        if (m_vy > MAX_FALL_SPEED) m_vy = MAX_FALL_SPEED;
        if (m_state != DecadeState::DimensionKick && m_state != DecadeState::AttackPunch && m_state != DecadeState::AttackSlash) {
            SetState(m_vy < 0 ? DecadeState::Jump : DecadeState::Fall);
        }
    }

    // Integrate position
    m_x += m_vx * dt;
    m_y += m_vy * dt;

    // Temporary ground floor check (until TileMap collision is active)
    float groundY = 380.0f;
    if (m_y >= groundY) {
        m_y = groundY;
        m_vy = 0.0f;
        m_isGrounded = true;
    }

    // Update active animation
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
        Animation* anim = m_animations[currentAnim];
        anim->Update(dt);

        // Check if non-looping animation finished
        if (anim->IsFinished()) {
            if (m_state == DecadeState::AttackPunch ||
                m_state == DecadeState::AttackSlash ||
                m_state == DecadeState::AttackShoot ||
                m_state == DecadeState::DimensionKick ||
                m_state == DecadeState::Hurt) {
                SetState(m_isGrounded ? DecadeState::Idle : DecadeState::Fall);
            }
        }
    }
}

void Decade::Render(LPD3DXSPRITE spriteHandler) {
    if (!m_isActive) return;

    // Render Dimension Kick cards tunnel
    if (m_state == DecadeState::DimensionKick && m_cardSprite) {
        RECT cardRect = { 59, 10, 150, 200 };
        for (const auto& card : m_dimensionCards) {
            if (!card.isShattered) {
                m_cardSprite->Draw(spriteHandler, card.x, card.y, &cardRect, m_flipX, 0.7f, D3DCOLOR_ARGB(200, 255, 255, 255));
            }
        }
    }

    // Render Decade
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
        m_animations[currentAnim]->Render(spriteHandler, m_x, m_y, m_flipX, 0.6f);
    }
}

bool Decade::IsAttacking() const {
    return (m_state == DecadeState::AttackPunch ||
            m_state == DecadeState::AttackSlash ||
            m_state == DecadeState::AttackShoot ||
            m_state == DecadeState::DimensionKick);
}

bool Decade::IsInvulnerable() const {
    return (m_state == DecadeState::DimensionKick || m_state == DecadeState::Block);
}

void Decade::TakeDamage(int damage) {
    if (IsInvulnerable() || m_state == DecadeState::Defeat) return;

    if (m_state == DecadeState::Block) {
        damage = (int)(damage * 0.2f); // 80% damage reduction when blocking
    }

    m_health -= damage;
    if (m_health <= 0) {
        m_health = 0;
        SetState(DecadeState::Defeat);
    } else {
        SetState(DecadeState::Hurt);
    }
}
