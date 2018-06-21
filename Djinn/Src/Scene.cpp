#include "Scene.h"

//using namespace std;
using namespace Djinn;

Scene::Scene()
{
    mainCamera = new Camera(this);

    // TODO: for memory efficiency, we'll only load meshes to CPU memory on load
    // then release them once they've been uploaded to the GPU.

    staticGeo.push_back(new Mesh());
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


std::vector<Camera *> Scene::GetCameras()
{
    return cameras;
}


std::vector<Mesh *> Scene::GetStaticGeo()
{
    return staticGeo;
}


std::vector<Mesh *> Scene::GetNonStaticGeo()
{
    return nonStaticGeo;
}

