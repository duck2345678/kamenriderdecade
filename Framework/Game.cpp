#include "Game.h"

Game* Game::s_instance = nullptr;

Game::Game()
    : m_hWnd(NULL), m_hInstance(NULL), m_d3d(NULL), m_d3ddev(NULL),
      m_spriteHandler(NULL), m_isRunning(false), m_timeScale(1.0f) {}

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

void Game::Update(float dt) {
    // TODO: Update SceneManager & Input
}

void Game::Render() {
    if (!m_d3ddev) return;

    // Clear buffer (Dark background)
    m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(20, 20, 30), 1.0f, 0);

    if (SUCCEEDED(m_d3ddev->BeginScene())) {
        m_spriteHandler->Begin(D3DXSPRITE_ALPHABLEND);

        // TODO: Render Current Scene

        m_spriteHandler->End();
        m_d3ddev->EndScene();
    }

    m_d3ddev->Present(NULL, NULL, NULL, NULL);
}

void Game::CleanUp() {
    if (m_spriteHandler) { m_spriteHandler->Release(); m_spriteHandler = NULL; }
    if (m_d3ddev) { m_d3ddev->Release(); m_d3ddev = NULL; }
    if (m_d3d) { m_d3d->Release(); m_d3d = NULL; }
}
