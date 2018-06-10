#include "Scene.h"

using namespace Djinn;

Scene::Scene()
{
    auto defaultCamera = new Camera(this);
    cameras.push_back(defaultCamera);

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

