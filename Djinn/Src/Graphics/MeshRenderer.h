#pragma once

#include "Mesh.h"

namespace Djinn
{
    class MeshRenderer
    {
    public:
        MeshRenderer();
        ~MeshRenderer();
        Mesh *mesh;
    };
}