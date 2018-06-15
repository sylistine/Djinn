#pragma once

#include "Scene.h"

namespace Djinn {
    class Scene;

    /// This class contains the logic manages the size and shape of the render target,
    /// and also controls filtering meshes from the current scene.
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