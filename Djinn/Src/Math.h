#pragma once

#include <cmath>
#include <DirectXMath.h>
#include <wincrypt.h>

using namespace DirectX;

namespace Djinn
{
    namespace Math
    {
        const float PI = 3.1415926535f;
        const float PI_DIVIDED_BY_TWO = PI / 2.0f;

        XMFLOAT4X4 Identity()
        {
            XMFLOAT4X4 I(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            );
            return I;
        }

        inline float cot (const float& theta)
        {
            return tan(PI_DIVIDED_BY_TWO - theta);
        }

        // Short for cofactor. Provides the multiplicative inverse of a (re: 1 / a)
        inline float cof (const float& a)
        {
            return 1.0f / a;
        }

        // purely for reference only. Use XMMatrixPerspectiveFovLH() or ...RH()
        XMFLOAT4X4 Perspective(float n, float f, float fov, float r)
        {
            const float t = tan(fov / 2.0f);
            const float a = f / (f - n);
            const float b = -f * n / (f - n);
            XMFLOAT4X4 persp(
                cof(r*t), 0.0f,   0.0f, 0.0f,
                0.0f,     cof(t), 0.0f, 0.0f,
                0.0f,     0.0f,   a,    1.0f,
                0.0f,     0.0f,   b,    0.0f
            );

            return persp;
        }
    }
}
