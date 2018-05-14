#pragma once

#include <cmath>
#include <DirectXMath.h>

using namespace DirectX;

namespace Djinn
{
    class Math
    {
        // cmath extensions.
        inline float cot(const float& theta);

        inline float oneOver(const float& a);

        // Vector math extensions.
        inline float Dot(FXMVECTOR a, FXMVECTOR b);

        // Vector math abstractions.
        inline float Magnitude(FXMVECTOR vector);

        XMVECTOR Proj(FXMVECTOR v, FXMVECTOR n);

        // Matrix math abstractions.
        XMFLOAT4X4 Identity();

        // Use XMMatrixLookAtLH()
        XMFLOAT4X4 ViewTransform(
            FXMVECTOR eyePos, FXMVECTOR lookDir, FXMVECTOR up);

        // Use XMMatrixPerspectiveFovLH() or ...RH()
        XMFLOAT4X4 Perspective(
            const float near,
            const float far,
            const float fov,
            const float aspect);
    };
}
