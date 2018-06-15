#pragma once

#include "Vertex.h"

namespace Djinn {
    class Transform {
    public:
        Transform();
        Transform(const Transform& other);
        ~Transform();

        Vertex position;
    };
}