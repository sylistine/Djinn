#pragma once

#include "GfxRHI.h"
#include "Mesh.h"
#include "../Color.h"
#include "../Timer.h"
#include "../Scene.h"

namespace Djinn {
    class Graphics {
    public:
        Graphics(GfxRHI *gfxRHI);
        ~Graphics();
        void SetCurrentScene(Scene *scene);
        void Update();
    private:
        GfxRHI *gfxRHI;
        CommandBuffer *commandBuffer;
        Scene *scene;
        void SetupGeo();
        void UpdateNonStaticGeo();
    };
}
