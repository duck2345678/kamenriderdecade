#include "Engine/Core/Game.h"
#include "Game/Entities/Decade.h"
#include "Game/Entities/LevelActors.h"
#include "Engine/Graphics/Camera.h"
#include "Game/World/TileMap.h"
#include "Game/World/ParallaxBackground.h"
#include "Engine/Graphics/TextureManager.h"
#include <cmath>

Game* Game::s_instance = nullptr;

Game::Game()
    : m_hWnd(NULL), m_hInstance(NULL), m_d3d(NULL), m_d3ddev(NULL),
      m_spriteHandler(NULL), m_isRunning(false), m_timeScale(1.0f),
      m_decade(nullptr), m_camera(nullptr), m_tileMap(nullptr), m_background(nullptr),
      m_font(nullptr), m_stageClear(false), m_respawnTimer(0.0f),
      m_spawnX(STAGE_SPAWN_X), m_spawnY(STAGE_SPAWN_Y) {}

Game::~Game() {
    CleanUp();
}

Game* Game::GetInstance() {
    if (s_instance == nullptr) {
        s_instance = new Game();
    }
    return s_instance;
}

LRESULT CALLBACK Game::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

bool Game::Initialize(HINSTANCE hInstance, int nCmdShow) {
    m_hInstance = hInstance;

    // 1. Register Window Class
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = Game::WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = L"KamenRiderDecadeWindowClass";

    if (!RegisterClassEx(&wc)) return false;

    // 2. Create Window
    RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    m_hWnd = CreateWindowEx(
        0,
        L"KamenRiderDecadeWindowClass",
        GAME_TITLE,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left,
        wr.bottom - wr.top,
        NULL, NULL, hInstance, NULL
    );

    if (!m_hWnd) return false;

    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);

    // 3. Initialize Direct3D
    m_d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!m_d3d) return false;

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = m_hWnd;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferWidth = SCREEN_WIDTH;
    d3dpp.BackBufferHeight = SCREEN_HEIGHT;

    HRESULT hr = m_d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        m_hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp,
        &m_d3ddev
    );

    if (FAILED(hr)) return false;

    // 4. Initialize Sprite Handler
    hr = D3DXCreateSprite(m_d3ddev, &m_spriteHandler);
    if (FAILED(hr)) return false;

    // 5. Initialize Camera
    m_camera = new Camera(SCREEN_WIDTH, SCREEN_HEIGHT);
    m_camera->SetMapLimits(MAP_PIXEL_WIDTH, MAP_PIXEL_HEIGHT);

    // 6. Initialize Parallax Background
    m_background = new ParallaxBackground();
    m_background->Init(m_d3ddev);

    // 7. Initialize TileMap
    m_tileMap = new TileMap();
    m_tileMap->Init(m_d3ddev);

    // 8. Initialize Decade on the stage spawn.
    m_decade = new Decade(m_spawnX, m_spawnY);
    m_decade->InitAnimations(m_d3ddev);

    int enemyCount = 0;
    const EnemySpawn* enemies = GetEnemySpawns(enemyCount);
    for (int i = 0; i < enemyCount; ++i) {
        Shocker* grunt = new Shocker(enemies[i].x, enemies[i].surfaceTop, enemies[i].left, enemies[i].right);
        grunt->Init(m_d3ddev);
        m_shockers.push_back(grunt);
    }

    int pickupCount = 0;
    const PickupSpawn* pickups = GetPickupSpawns(pickupCount);
    for (int i = 0; i < pickupCount; ++i) {
        CardPickup* card = new CardPickup(pickups[i].x, pickups[i].y);
        card->Init(m_d3ddev);
        m_pickups.push_back(card);
    }

    D3DXCreateFontW(m_d3ddev, 22, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Arial", &m_font);

    m_isRunning = true;
    return true;
}

void Game::Run() {
    MSG msg;
    DWORD frameStart = GetTickCount();
    DWORD tickPerFrame = 1000 / FRAME_PER_SECOND;

    while (m_isRunning) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        DWORD now = GetTickCount();
        DWORD dt = now - frameStart;

        if (dt >= tickPerFrame) {
            frameStart = now;
            float deltaTime = (float)dt / 1000.0f;

            // Apply timescale (Clock Up)
            Update(deltaTime * m_timeScale);
            Render();
        } else {
            Sleep(tickPerFrame - dt);
        }
    }
}

namespace {
bool BoxesOverlap(float ax, float ay, float aw, float ah, float bx, float by, float bw, float bh) {
    return std::fabs(ax - bx) * 2.0f < (aw + bw) && std::fabs(ay - by) * 2.0f < (ah + bh);
}
}

void Game::Update(float dt) {
    if (!m_decade) return;

    const bool defeated = m_decade->GetCurrentState() == DecadeState::Defeat;
    if (!defeated) {
        m_decade->Update(dt, m_tileMap);

        if (m_decade->GetY() > MAP_PIXEL_HEIGHT + 24.0f) {
            m_decade->Kill();
        }

        if (!m_stageClear && m_decade->GetX() >= STAGE_GOAL_X &&
            m_decade->GetCurrentState() != DecadeState::Defeat) {
            m_stageClear = true;
            m_decade->LockControls();
        }

        if (m_decade->GetX() >= STAGE_CHECKPOINT_X && m_decade->GetY() < STAGE_SPAWN_Y + 8.0f) {
            m_spawnX = STAGE_CHECKPOINT_X + 32.0f;
            m_spawnY = STAGE_SPAWN_Y;
        }
    }

    if (m_decade->GetCurrentState() == DecadeState::Defeat && !m_stageClear) {
        m_respawnTimer += dt;
        if (m_respawnTimer > 1.35f) {
            m_respawnTimer = 0.0f;
            m_decade->Respawn(m_spawnX, m_spawnY);
            for (Shocker* grunt : m_shockers) grunt->Reset();
            for (CardPickup* card : m_pickups) card->Reset();
        }
    } else {
        m_respawnTimer = 0.0f;
    }

    const bool playing = !m_stageClear && m_decade->GetCurrentState() != DecadeState::Defeat;
    if (playing) {
        for (Shocker* grunt : m_shockers) {
            grunt->Update(dt, m_tileMap, m_decade->GetX(), m_decade->GetY());
        }

        for (CardPickup* card : m_pickups) {
            card->Update(dt);
            if (card->IsTaken()) continue;
            if (BoxesOverlap(m_decade->GetX(), m_decade->GetY(), 24.0f, 48.0f,
                             card->GetX(), card->GetY(), 18.0f, 28.0f)) {
                card->Take();
                m_decade->Heal(35);
            }
        }

        for (Shocker* grunt : m_shockers) {
            if (!grunt->IsTargetable()) continue;
            if (m_decade->IntersectsAttack(grunt->GetX(), grunt->GetY(), 22.0f, 44.0f)) {
                grunt->TakeHit(m_decade->AttackDamage(), m_decade->GetAttackSerial(), m_decade->GetX());
            }
            if (grunt->IsTargetable() &&
                grunt->AttackHits(m_decade->GetX(), m_decade->GetY(), 24.0f, 48.0f)) {
                m_decade->TakeDamage(14);
            }
        }
    }

    if (m_camera) {
        m_camera->Update(m_decade->GetX(), m_decade->GetY(), dt);
    }
}

void Game::Render() {
    if (!m_d3ddev) return;

    m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(10, 10, 20), 1.0f, 0);

    if (SUCCEEDED(m_d3ddev->BeginScene())) {
        m_spriteHandler->Begin(D3DXSPRITE_ALPHABLEND);

        // 1. Draw Parallax Background layers
        if (m_background && m_camera) {
            m_background->Render(m_spriteHandler, m_camera);
        }

        // 2. Draw TileMap
        if (m_tileMap && m_camera) {
            m_tileMap->Render(m_spriteHandler, m_camera);
        }

        for (CardPickup* card : m_pickups) {
            card->Render(m_spriteHandler, m_camera);
        }
        for (Shocker* grunt : m_shockers) {
            grunt->Render(m_spriteHandler, m_camera);
        }

        // 3. Draw Decade
        if (m_decade && m_camera) {
            m_decade->Render(m_spriteHandler, m_camera);
        }

        m_spriteHandler->End();

        if (m_font && m_decade) {
            wchar_t hud[64];
            wsprintfW(hud, L"HP  %d", m_decade->GetHealth());
            RECT hudRect = { 16, 14, 280, 48 };
            m_font->DrawTextW(NULL, hud, -1, &hudRect, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(255, 140, 255, 230));

            if (m_stageClear) {
                RECT title = { 0, 168, SCREEN_WIDTH, 220 };
                RECT sub = { 0, 214, SCREEN_WIDTH, 260 };
                m_font->DrawTextW(NULL, L"STAGE CLEAR", -1, &title, DT_CENTER | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 230, 120));
                m_font->DrawTextW(NULL, L"The gate is open.", -1, &sub, DT_CENTER | DT_NOCLIP, D3DCOLOR_ARGB(255, 220, 245, 255));
            }
        }

        m_d3ddev->EndScene();
    }

    m_d3ddev->Present(NULL, NULL, NULL, NULL);
}

void Game::CleanUp() {
    for (Shocker* grunt : m_shockers) delete grunt;
    m_shockers.clear();
    for (CardPickup* card : m_pickups) delete card;
    m_pickups.clear();

    if (m_font) { m_font->Release(); m_font = nullptr; }

    if (m_decade) {
        delete m_decade;
        m_decade = nullptr;
    }
    if (m_tileMap) {
        delete m_tileMap;
        m_tileMap = nullptr;
    }
    if (m_background) {
        delete m_background;
        m_background = nullptr;
    }
    if (m_camera) {
        delete m_camera;
        m_camera = nullptr;
    }

    TextureManager::GetInstance()->Clear();

    if (m_spriteHandler) { m_spriteHandler->Release(); m_spriteHandler = NULL; }
    if (m_d3ddev) { m_d3ddev->Release(); m_d3ddev = NULL; }
    if (m_d3d) { m_d3d->Release(); m_d3d = NULL; }
}
