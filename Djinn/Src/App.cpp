#include "App.h"

using namespace Djinn;

App::App(AppWrapper *appWrapper)
    : appWrapper(appWrapper)
    , defaultScene()
    , graphics(appWrapper->GetGfxRHI())
{
}


App::~App()
{
}


void App::Start()
{
    graphics.SetCurrentScene(&defaultScene);
}


void App::Update()
{
    graphics.Update();
}
