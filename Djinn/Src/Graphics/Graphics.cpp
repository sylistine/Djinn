#include "Graphics.h"

using namespace Djinn;

Graphics::Graphics(GfxRHI *gfxRHI)
    : gfxRHI(gfxRHI)
{

}


Graphics::~Graphics()
{

}


void Graphics::SetCurrentScene(Scene *currentScene)
{
    // cleanup old scene geomtry.
    scene = currentScene;
    SetupGeo();
}


void Graphics::Update()
{
    // Basically short circuiting the logic here because we haven't updated
    // the gfxRHI to provide an interface for this controller to create command buffs
    // or upload verts and textures.
    gfxRHI->Draw();

    // Dummy call, for future implementation.
    //UpdateNonStaticGeo();
}


/// Sets up both static and non-static geomtry.
void Graphics::SetupGeo()
{
    for (auto& camera : scene->GetCurrentCameras())
    {
        
    }
}

void Graphics::UpdateNonStaticGeo()
{
    for (auto& camera : scene->GetCurrentCameras())
    {

    }
}
