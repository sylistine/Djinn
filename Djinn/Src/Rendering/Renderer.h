#pragma once
class Renderer
{
public:
    virtual ~Renderer() = default;
    virtual bool Initialize() = 0;
    virtual void OnResize() = 0;
    virtual bool GetMsaa4xState() = 0;
    virtual void SetMsaa4xState(bool) = 0;
    virtual void SetClientDimensions(int, int) = 0;
    bool MsaaStateQ = false;
};

