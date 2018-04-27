#pragma once

#include <string>

using namespace std;

class Timer
{
public:
    Timer();
    float TotalTime()const;
    float DeltaTime()const;
    void Reset();
    void Start();
    void Stop();
    void Tick();
    void UpdateFrameStats();
    wstring FrameStatus() const;
    float Fps() const;
    float Mspf() const;
private:
    double secondsPerCount;
    double deltaTime;
    __int64 baseTime;
    __int64 stopTime;
    __int64 pausedTime;
    __int64 prevTime;
    __int64 currTime;
    float fps;
    float mspf;
    bool stopped;
};
