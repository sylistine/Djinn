#pragma once

#include <cassert>

#include <Windows.h>
#include <WindowsX.h>

//#if DIRECTX
#include "Rendering/D3DRenderer.h"
//#endif
#include "Timer.h"

class WindowsApp {
public:
    // tors
    WindowsApp(HINSTANCE hInstance);
    ~WindowsApp();
    // statics
    static WindowsApp *app;
    static WindowsApp *GetApp();
    //publics
    bool Initialize();
    int Run();
    LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);
private:
    // Windows parameters.
    HINSTANCE hInstance;
    HWND hWnd{};
    Timer timer;
    Renderer *renderer;
    // Initialization.
    bool initialized = false;
    bool InitializeWindow();
    // Game logic
    void Update();
    void OnMouseDown(WPARAM, int, int);
    void OnMouseUp(WPARAM, int, int);
    void OnMouseMove(WPARAM, int, int);
    // game state
    int clientWidth = 800;
    int clientHeight = 600;
    bool paused = false;
    bool minimized = false;
    bool maximized = false;
    bool resizing = false;
};
