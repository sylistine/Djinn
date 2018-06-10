#pragma once

namespace Djinn
{
    class Color {
    public:
        static void Hsv2Rgb(float h, float s, float v, float& r, float& g, float& b);
    private:
        Color() = default;
        ~Color() = default;
    };
}