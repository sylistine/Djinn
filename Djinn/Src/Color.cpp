#include "Color.h"

#include <cmath>

using namespace Djinn;

void Color::Hsv2Rgb(float h, float s, float v, float& r, float& g, float& b)
{
    float c = v * s;
    float m = v - c;
    float xprime = fmod((h / 60.f), 2.f) - 1.f;
    if (xprime < 0) xprime = -xprime;
    float x = c * (1.f - xprime);

    if (0 <= h && h < 60) {
        r = c; g = x; b = 0;
    }
    else if (60 <= h && h < 120) {
        r = x; g = c; b = 0;
    }
    else if (120 <= h && h < 180) {
        r = 0; g = c; b = x;
    }
    else if (180 <= h && h < 240) {
        r = 0; g = x; b = c;
    }
    else if (240 <= h && h < 300) {
        r = x; g = 0; b = c;
    }
    else {
        r = c; g = 0; b = x;
    }

    r = (r + m) * 255.f / 255.f;
    g = (g + m) * 255.f / 255.f;
    b = (b + m) * 255.f / 255.f;
}
