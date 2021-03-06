#pragma once

#include "../Transform.h"

namespace Djinn
{
    class CommandBuffer {
    public:
        virtual ~CommandBuffer() = default;
        virtual void Initialize() = 0;
        virtual void Draw() = 0;
        virtual void UpdateViewMatrix(Transform *cameraTransform) = 0;
        virtual void SetStaticGeometry(
            Vertex *verts,
            unsigned int vertc, 
            unsigned int *indices, 
            unsigned int indexc) = 0;
    };
}
