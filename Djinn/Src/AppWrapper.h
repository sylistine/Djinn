#pragma once

#include "Graphics/GfxRHI.h"

namespace Djinn {
    class AppWrapper
    {
    public:
        AppWrapper() = default;
        virtual ~AppWrapper() = default;
        virtual GfxRHI *GetGfxRHI() = 0;
    };
}
