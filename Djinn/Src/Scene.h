#pragma once

#include "stdinc.h"

#include <vector>

#include "Graphics/Mesh.h"
#include "Camera.h"
#include "Graphics/MeshRenderer.h"

namespace Djinn {
    class Camera;

    class Scene
    {
    public:
        Scene();
        Scene(const Scene& other);
        ~Scene();
        void Initialize();
        std::vector<Camera *> GetCameras();
        std::vector<Mesh *> GetStaticGeo();
        std::vector<Mesh *> GetNonStaticGeo();
    private:
        std::vector<Camera *> cameras;
        std::vector<Mesh *> staticGeo;
        std::vector<Mesh *> nonStaticGeo;
    };
}
