#pragma once

#include "Scene.h"

namespace Djinn {
    class Scene;

    class Camera {
    public:
        Camera(Scene *parentScene);
        ~Camera();
        std::vector<Mesh *> GetRenderableGeo();
    private:
        Scene * parentScene;
        Camera(const Camera& other);
    };
}