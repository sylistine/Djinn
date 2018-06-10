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
    gfxRHI->PrepareMainCommandBuffer();
    UpdateNonStaticGeo();
}


/// Uploads all geometry in the scene to a VBO.
void Graphics::SetupGeo()
{
    for (auto& camera : scene->GetCameras())
    {
        auto renderableGeo = camera->GetRenderableGeo();
    }
}

void Graphics::UpdateNonStaticGeo()
{
    for (auto& camera : scene->GetCameras())
    {
        // Update moving geo.
    }
}
