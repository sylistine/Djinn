#pragma once

class Renderer
{
public:
    virtual ~Renderer() = default;
    virtual bool Initialize() = 0;
    virtual bool GetMsaa4xState() = 0;
    virtual void SetMsaa4xState(bool) = 0;
    virtual void SetClientDimensions(int width, int height) = 0;
    virtual void Draw() = 0;
    bool initialized = false;
};

