#pragma once

#include <cmath>
#include <DirectXMath.h>

namespace Djinn
{
    /// Includes missing extensions to existing classes.
    /// For somewhat non-mathematical math utilities, see MathUtil.h
    class Math
    {
    public:
        // cmath extensions.
        inline float cot(const float& theta);

        inline float oneOver(const float& a);

        // Vector math extensions.
        inline float Dot(DirectX::FXMVECTOR a, DirectX::FXMVECTOR b);

        // Vector math abstractions.
        inline float Magnitude(DirectX::FXMVECTOR vector);

        DirectX::XMVECTOR Proj(DirectX::FXMVECTOR v, DirectX::FXMVECTOR n);

        // Matrix math abstractions.
        static DirectX::XMFLOAT4X4 Identity();

        // Use XMMatrixLookAtLH()
        static DirectX::XMMATRIX ViewMatrix(
            DirectX::FXMVECTOR eyePos, DirectX::FXMVECTOR lookDir, DirectX::FXMVECTOR up);

        // Use XMMatrixPerspectiveFovLH() or ...RH()
        static DirectX::XMMATRIX PerspectiveMatrix(
            const float near,
            const float far,
            const float fov,
            const float aspect);
    };
}
