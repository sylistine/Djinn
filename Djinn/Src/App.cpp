#include "App.h"

using namespace Djinn;

App::App(AppWrapper *appWrapper)
    : appWrapper(appWrapper), graphics(nullptr)
{
    graphics = new Graphics(appWrapper->GetGfxRHI());
}


/// Private cctor.
App::App(const App& other)
{
    // Do nothing. Not allowed.
}


App::~App()
{
}


void App::Start()
{
    // Load default scene.
}


void App::Update()
{
    graphics->Update();
}
