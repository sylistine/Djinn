#pragma once

#include "../stdinc.h"

class MsaaQuality{
public:
    MsaaQuality(uint sampleCount);
    ~MsaaQuality();

private:
    uint sampleCount;

};