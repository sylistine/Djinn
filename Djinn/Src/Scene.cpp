#include "Scene.h"

using namespace Djinn;

Scene::Scene() : cube()
{
    cameras.push_back(new Camera);
}

Scene::Scene(const Scene& other)
    : cube(other.cube)
{

}


Scene::~Scene()
{
}


/// Load scene objects from serialized.
void Scene::Initialize()
{
    
}


std::vector<Camera *> Scene::GetCurrentCameras()
{
    return cameras;
}
