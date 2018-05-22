#pragma once

#include "AppWrapper.h"
#include "Graphics/Graphics.h"

namespace Djinn {
    class App
    {
    public:
        App(AppWrapper *appWrapper);
        ~App();
        void Start();
        void Update();
    private:
        App(const App& other);
        AppWrapper *appWrapper;
        Graphics *graphics;
    };
}
