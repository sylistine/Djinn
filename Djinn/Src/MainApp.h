#pragma once

#include "AppWrapper.h"
#include "Rendering/Renderer.h"

class MainApp
{
public:
    MainApp(AppWrapper *appWrapper);
    ~MainApp();
    // Game logic
    void Update();
private:
    AppWrapper * appWrapper;
    Renderer * renderer;
    void GameUpdate();
};
