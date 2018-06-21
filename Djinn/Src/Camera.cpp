#include "Camera.h"

//using namespace std;
using namespace Djinn;

Camera::Camera(Scene *parentScene) : parentScene(parentScene)
{

}


/// Private.
Camera::Camera(const Camera& other)
{
}


Camera::~Camera()
{

}


std::vector<Mesh *> Camera::GetStaticGeo()
{
    return parentScene->GetStaticGeo();
}


Transform *Camera::GetTransform()
{
    return &transform;
}
