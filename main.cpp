#include <windows.h>
#include "Framework/Game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    Game* game = Game::GetInstance();

    if (!game->Initialize(hInstance, nCmdShow)) {
        MessageBox(NULL, L"Failed to initialize DirectX Game Engine!", L"Error", MB_ICONERROR);
        return -1;
    }

    game->Run();

    return 0;
}
