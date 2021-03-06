#include "WindowsAppWrapper.h"

using namespace Djinn;

LRESULT WINAPI MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return WindowsAppWrapper::GetApp()->WndProc(hWnd, msg, wParam, lParam);
}


WindowsAppWrapper::WindowsAppWrapper(HINSTANCE hInstance) : AppWrapper(), hInstance(hInstance)
{
    assert(windowsApp == nullptr);
    windowsApp = this;
}


WindowsAppWrapper::~WindowsAppWrapper() = default;


WindowsAppWrapper *WindowsAppWrapper::windowsApp = nullptr;


WindowsAppWrapper *WindowsAppWrapper::GetApp()
{
    return windowsApp;
}


bool WindowsAppWrapper::Initialize()
{
    if (!initialized)
    {
        if (!InitializeWindow())
        {
            return false;
        }

        // default to dx12rhi. Later serialize user preference and instantiate that.
        gfxRHI = new DX12RHI(hWnd, windowWidth, windowHeight);
        gfxRHI->Initialize();

        app = new App(this);
        app->Start();

        initialized = true;
    }

    return true;
}


bool WindowsAppWrapper::InitializeWindow()
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
    RECT rect = { 0, 0, windowWidth, windowHeight };
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


int WindowsAppWrapper::Run()
{
    MSG msg = { 0 };
    Timer::GetTimer()->Reset();

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Timer::GetTimer()->Tick();
            if (!paused)
            {
                Timer::GetTimer()->UpdateFrameStats();

                // TODO: Update and Draw should be left up to a platform agnostic Game or App class.
                app->Update();
            }
            else
            {
                Sleep(16);
            }
        }
    }

    return static_cast<int>(msg.wParam);
}


GfxRHI *WindowsAppWrapper::GetGfxRHI()
{
    return gfxRHI;
}


void WindowsAppWrapper::OnMouseDown(WPARAM wParam, int x, int y)
{
    
}


void WindowsAppWrapper::OnMouseUp(WPARAM wParam, int x, int y)
{
    
}


void WindowsAppWrapper::OnMouseMove(WPARAM wParam, int x, int y)
{
    
}


LRESULT WindowsAppWrapper::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
            Timer::GetTimer()->Stop();
            paused = true;
        } else
        {
            Timer::GetTimer()->Start();
            paused = false;
        }
        return 0;
    case WM_SIZE:
        windowWidth  = LOWORD(lParam);
        windowHeight = HIWORD(lParam);
        if (gfxRHI != nullptr && gfxRHI->IsInitialized())
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
                gfxRHI->SetClientDimensions(windowWidth, windowHeight);
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
                    gfxRHI->SetClientDimensions(windowWidth, windowHeight);
                }
            }
        }
        return 0;
    case WM_ENTERSIZEMOVE:
        paused = true;
        resizing = true;
        Timer::GetTimer()->Stop();
        return 0;
    case WM_EXITSIZEMOVE:
        paused = false;
        resizing = false;
        Timer::GetTimer()->Start();
        if (gfxRHI != nullptr)
        {
            gfxRHI->SetClientDimensions(windowWidth, windowHeight);
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
        // We only capture esc and f2 right now.
        if (wParam == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }
        else if (wParam == VK_F2)
        {
            auto currentSampleLevel = static_cast<int>(gfxRHI->GetMsaaSampleLevel());
            auto newSampleLevel     = static_cast<MSAA_SAMPLE_LEVEL>((currentSampleLevel + 1) % 3);
            gfxRHI->SetMsaaSampleLevel(newSampleLevel);
        }
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
