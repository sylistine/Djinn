#include "WindowsApp.h"

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR cmdLIne,
    int cmdShow)
{
    WindowsApp app(hInstance);
    if (!app.Initialize())
    {
        return -1;
    }
    app.Run();

    return 0;
}
