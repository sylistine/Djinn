#pragma once

#include <cmath>
#include <DirectXMath.h>
#include <wincrypt.h>

using namespace DirectX;

namespace Djinn
{
    class Math
    {
        const float PI = 3.1415926535f;
        const float PI_DIVIDED_BY_TWO = PI / 2.0f;

        inline float cot(const float& theta);

        inline float oneOver(const float& a);

        // Vector math abstractions.
        inline float Magnitude(const XMVECTOR& vector);

        XMVECTOR Proj(const XMVECTOR v, const XMVECTOR n);

        // Matrix math abstractions.
        XMFLOAT4X4 Identity();

        // Use XMMatrixPerspectiveFovLH() or ...RH()
        XMFLOAT4X4 Perspective(
            const float near,
            const float far,
            const float fov,
            const float aspect);

        // Use XMMatrixLookAtLH()
        XMFLOAT4X4 ViewTransform(
            const XMVECTOR& eyePos, const XMVECTOR& lookDir, const XMVECTOR& up);
    };
}
