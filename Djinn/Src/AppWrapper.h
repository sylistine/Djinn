#pragma once

#include "Rendering/Renderer.h"

class AppWrapper
{
public:
    AppWrapper() = default;
    virtual ~AppWrapper() = default;
    virtual Renderer *GetRenderer() = 0;
};

