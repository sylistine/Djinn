#include "Scene.h"

using namespace std;
using namespace Djinn;

Scene::Scene()
{
    // the main camera will always be cameras[0]
    mainCamera = new Camera(this);

    // TODO: for memory efficiency, we'll only load meshes to CPU memory on load
    // then release them once they've been uploaded to the GPU.
    auto cubeMesh = new Mesh();
    staticGeo.push_back(cubeMesh);
}

Scene::Scene(const Scene& other)
{
}


Scene::~Scene()
{
    for (auto& camera : cameras)
    {
        delete camera;
    }

    for (auto& mesh : staticGeo)
    {
        delete mesh;
    }

    for (auto& mesh : nonStaticGeo)
    {
        delete mesh;
    }
}


/// Load scene objects from serialized.
void Scene::Initialize()
{
    
}


Camera *Scene::GetMainCamera()
{
    return mainCamera;
}


vector<Camera *> Scene::GetCameras()
{
    return cameras;
}


vector<Mesh *> Scene::GetStaticGeo()
{
    return staticGeo;
}


vector<Mesh *> Scene::GetNonStaticGeo()
{
    return nonStaticGeo;
}

