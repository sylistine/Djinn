#include "Windows/WindowsAppWrapper.h"

using namespace Djinn;

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR cmdLIne,
    int cmdShow)
{
    WindowsAppWrapper app(hInstance);
    if (!app.Initialize())
    {
        return -1;
    }
    app.Run();

    return 0;
}
