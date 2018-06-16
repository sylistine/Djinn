#include "Camera.h"

using namespace std;
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


vector<Mesh *> Camera::GetRenderableGeo()
{
    vector<Mesh *> renderableGeo;
    for (auto mesh : parentScene->GetStaticGeo())
    {
        // TODO: filter results
        renderableGeo.push_back(mesh);
    }
    return renderableGeo;
}


Transform *Camera::GetTransform()
{
    return &transform;
}
