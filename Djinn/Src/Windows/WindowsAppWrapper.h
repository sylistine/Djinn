#pragma once

#include <cassert>

#include <Windows.h>
#include <WindowsX.h>

#include "../Graphics/GfxRHI.h"
#include "../Graphics/DirectX/DX12RHI.h"
#include "../AppWrapper.h"
#include "../App.h"

#include "../Timer.h"

namespace Djinn {
    class WindowsAppWrapper : public AppWrapper
    {
    public:
        // tors
        WindowsAppWrapper(HINSTANCE hInstance);
        ~WindowsAppWrapper();
        // statics
        static WindowsAppWrapper *windowsApp;
        static WindowsAppWrapper *GetApp();
        //publics
        bool Initialize();
        int Run();
        GfxRHI *GetGfxRHI() override;
        LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);
    private:
        // Windows parameters.
        HINSTANCE hInstance;
        HWND hWnd;
        Timer timer;
        GfxRHI *gfxRHI;
        App *app;
        int windowWidth = 800;
        int windowHeight = 600;
        // Initialization.
        bool initialized = false;
        bool InitializeWindow();
        // WndProc message handlers.
        void OnMouseDown(WPARAM, int, int);
        void OnMouseUp(WPARAM, int, int);
        void OnMouseMove(WPARAM, int, int);
        // game state
        bool paused = false;
        bool minimized = false;
        bool maximized = false;
        bool resizing = false;
    };
}
