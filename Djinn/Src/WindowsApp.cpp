#include "WindowsApp.h"

LRESULT WINAPI MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return WindowsApp::GetApp()->WndProc(hWnd, msg, wParam, lParam);
}

WindowsApp::WindowsApp(HINSTANCE hInstance) : hInstance(hInstance)
{
    assert(app == nullptr);
    app = this;
//#if DIRECTX
    renderer = new D3DRenderer(hWnd, clientWidth, clientHeight);
//#endif
}

WindowsApp::~WindowsApp() = default;

WindowsApp *WindowsApp::app = nullptr;
WindowsApp *WindowsApp::GetApp()
{
    return app;
}

bool WindowsApp::Initialize()
{
    if (!initialized)
    {
        if (!InitializeWindow())
        {
            return false;
        }

        if (!renderer->Initialize())
        {
            return false;
        }

        initialized = true;
    }

    return true;
}

bool WindowsApp::InitializeWindow()
{
    const WCHAR* wndClassName = L"DjinnRendererWindow";
    const WCHAR* wndTitle = L"Djinn Renderer";

    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
    wc.lpszMenuName = 0;
    wc.lpszClassName = wndClassName;

    if (!RegisterClass(&wc))
    {
        MessageBox(0, L"RegisterClass Failed.", 0, 0);
        return false;
    }

    // Compute window rectangle dimensions based on requested client area dimensions.
    RECT rect = { 0, 0, clientWidth, clientHeight };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;

    hWnd = CreateWindow(
        wndClassName,
        wndTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width, height,
        0, 0,
        hInstance,
        0);
    if (!hWnd)
    {
        MessageBox(0, L"CreateWindow Failed.", 0, 0);
        return false;
    }

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    return true;
}

int WindowsApp::Run()
{
    MSG msg = { 0 };
    timer.Reset();

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            timer.Tick();
            if (!paused)
            {
                timer.UpdateFrameStats();
                Update();
            }
            else
            {
                Sleep(16);
            }
        }
    }

    return static_cast<int>(msg.wParam);
}



void WindowsApp::Update()
{
}


void WindowsApp::OnMouseDown(WPARAM wParam, int x, int y)
{
    
}


void WindowsApp::OnMouseUp(WPARAM wParam, int x, int y)
{
    
}


void WindowsApp::OnMouseMove(WPARAM wParam, int x, int y)
{
    
}


LRESULT WindowsApp::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    /* Handling the following messages:
     * ACTIVATE
     * SIZE
     * ENTERSIZEMOVE
     * EXITSIZEMOVE
     * DESTROY
     * MENUCHAR
     * GETMINMAXINFO
     * L- M- RBUTTONDOWN
     * L- M- RBUTTONUP
     * MOUSEMOVE
     * KEYUP
     */

    switch(msg)
    {
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE)
        {
            timer.Stop();
            paused = true;
        } else
        {
            timer.Start();
            paused = false;
        }
        return 0;
    case WM_SIZE:
        clientWidth = LOWORD(lParam);
        clientHeight = HIWORD(lParam);
        if (renderer != nullptr)
        {
            if (wParam == SIZE_MINIMIZED)
            {
                paused = true;
                minimized = true;
                maximized = false;
            }
            else if (wParam == SIZE_MAXIMIZED)
            {
                paused = false;
                minimized = false;
                maximized = true;
                renderer->OnResize();
            }
            else if (wParam == SIZE_RESTORED)
            {
                if (resizing) {}
                else
                {
                    if (minimized)
                    {
                        paused = false;
                        minimized = false;
                    }
                    else if (maximized)
                    {
                        paused = false;
                        maximized = false;
                    }
                    renderer->OnResize();
                }
            }
        }
        return 0;
    case WM_ENTERSIZEMOVE:
        paused = true;
        resizing = true;
        timer.Stop();
        return 0;
    case WM_EXITSIZEMOVE:
        paused = false;
        resizing = false;
        timer.Start();
        if (renderer != nullptr)
        {
            renderer->OnResize();
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_MENUCHAR:
        return MAKELRESULT(0, MNC_CLOSE);
    case WM_GETMINMAXINFO:
        reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = 200;
        reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = 200;
        return 0;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
        OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    case WM_MOUSEMOVE:
        OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    case WM_KEYUP:
        if (wParam == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }
        else if (static_cast<int>(wParam) == VK_F2)
        {
            renderer->SetMsaa4xState(!renderer->MsaaStateQ);
        }
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
