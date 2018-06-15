#pragma once

namespace Djinn
{
    class Vertex
    {
    public:
        Vertex();
        Vertex(float x, float y, float z, float w = 0);
        ~Vertex();

        float x = 0;
        float y = 0;
        float z = 0;
        float w = 0;
    };
}
