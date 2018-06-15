#include "Graphics.h"

using namespace Djinn;

Graphics::Graphics(GfxRHI *gfxRHI)
    : gfxRHI(gfxRHI)
{
    commandBuffer = gfxRHI->GetCommandBuffer();
}


Graphics::~Graphics()
{

}


void Graphics::SetCurrentScene(Scene *currentScene)
{
    // TODO: cleanup old scene geomtry.
    scene = currentScene;

    // NOTE: Not a fan of this notation (arrow, arrow, dot).
    // Convert transform from copy to pointer?
    scene->GetMainCamera()->transform.position;
    SetupGeo();
}


void Graphics::Update()
{
    UpdateNonStaticGeo();
    commandBuffer->Draw();
}


void Graphics::SetupGeo()
{
    for (auto& camera : scene->GetCameras())
    {
        // Prepare swap chain and command buffers for each extra camera.
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
