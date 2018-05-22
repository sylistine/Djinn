#pragma once

#include "stdinc.h"

#include <vector>

#include "Graphics/Mesh.h"
#include "Camera.h"
#include "Graphics/MeshRenderer.h"

namespace Djinn {
    class Scene
    {
    public:
        Scene();
        Scene(const Scene& other);
        ~Scene();
        void Initialize();
        std::vector<Camera *> GetCurrentCameras();
    private:
        Mesh cube;
        std::vector<Camera *> cameras;
    };
}
