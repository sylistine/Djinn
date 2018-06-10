#pragma once

namespace Djinn
{
    class CommandBuffer {
    public:
        CommandBuffer();
        virtual ~CommandBuffer() = default;
        virtual void Reset() = 0;
        virtual void Clear(float *clearColor) = 0;
    };
}
