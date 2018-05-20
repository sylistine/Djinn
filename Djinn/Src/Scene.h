#pragma once

#include "stdinc.h"

#include "Rendering/Mesh.h"

class Scene
{
public:
    Scene();
    ~Scene();
    void Initialize();
private:
    Mesh cube;
};
