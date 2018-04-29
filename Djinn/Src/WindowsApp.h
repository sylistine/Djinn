#pragma once

#include <cassert>

#include <Windows.h>
#include <WindowsX.h>

#include "Rendering/D3DRenderer.h"

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
    HWND hWnd;
    Timer timer;
    Renderer *renderer;
    int windowWidth = 800;
    int windowHeight = 600;
    // Initialization.
    bool initialized = false;
    bool InitializeWindow();
    // Game logic
    void Update();
    void Draw();
    void OnMouseDown(WPARAM, int, int);
    void OnMouseUp(WPARAM, int, int);
    void OnMouseMove(WPARAM, int, int);
    // game state
    bool paused = false;
    bool minimized = false;
    bool maximized = false;
    bool resizing = false;
};
