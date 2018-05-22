#pragma once


typedef enum MSAA_SAMPLE_LEVEL {
    MSAA_SAMPLE_LEVEL_1X = 0,
    MSAA_SAMPLE_LEVEL_4X = 1,
    MSAA_SAMPLE_LEVEL_8X = 2
} MSAA_SAMPLE_LEVEL;

class GfxRHI
{
public:
    GfxRHI() = default;
    virtual ~GfxRHI() = default;
    virtual bool Initialize() = 0;
    virtual MSAA_SAMPLE_LEVEL GetMsaaSampleLevel() = 0;
    virtual void SetMsaaSampleLevel(MSAA_SAMPLE_LEVEL newLevel) = 0;
    virtual void SetClientDimensions(int width, int height) = 0;
    virtual void Draw() = 0;
    virtual bool IsInitialized() = 0;
};

