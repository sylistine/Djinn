#include "Graphics.h"

using namespace Djinn;

Graphics::Graphics(GfxRHI *gfxRHI)
    : gfxRHI(gfxRHI)
{
    commandBuffer = gfxRHI->GetCommandBuffer();
    commandBuffer->Initialize();
}


Graphics::~Graphics()
{

}


void Graphics::SetCurrentScene(Scene *currentScene)
{
    // TODO: cleanup old scene geomtry.
    scene = currentScene;

    SetupGeo();
}


void Graphics::Update()
{
    UpdateNonStaticGeo();

    commandBuffer->UpdateViewMatrix(scene->GetMainCamera()->GetTransform());
    commandBuffer->Draw();
}


void Graphics::SetupGeo()
{
    // NOTE: Meshes are currently stored raw in the scene, and have no transformation data.
    // So the only transformation that happens is when we move the camera.
    std::vector<Mesh *> staticGeometry = scene->GetMainCamera()->GetStaticGeo();

    // Assembling static geo into sequential memory.
    // Something will probably have to delete these at some point...
    UINT vertc = 0;
    UINT indc = 0;
    for (auto& geometry : staticGeometry)
    {
        vertc += geometry->VertexCount();
        indc += geometry->IndexCount();
    }
    Vertex *vertices = new Vertex[vertc];
    UINT *indices = new UINT[indc];
    int vertexOffset = 0;
    int indexOffset = 0;
    for (auto& geometry : staticGeometry)
    {
        memcpy(vertices + vertexOffset, geometry->Vertices(), geometry->VertexCount() * sizeof(Vertex));
        vertexOffset += geometry->VertexCount();
        memcpy(indices + indexOffset, geometry->Indices(), geometry->IndexCount() * sizeof(UINT));
        indexOffset += geometry->IndexCount();
    }

    commandBuffer->SetStaticGeometry(vertices, vertc, indices, indc);
}

void Graphics::UpdateNonStaticGeo()
{
}
