#include "Math.h"

using namespace Djinn;
using namespace DirectX;

// cmath extensions
inline float Math::cot(const float& theta)
{
    return tan(XM_PIDIV2 - theta);
}

inline float Math::oneOver(const float& a)
{
    return 1.0f / a;
}

// Vector math extensions.
inline float Math::Dot(FXMVECTOR a, FXMVECTOR b)
{
    // return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    return XMVectorGetX(XMVector3Dot(a, b));
}

// Vector math abstractions.
inline float Math::Magnitude(FXMVECTOR vector)
{
    return 0;
}

XMVECTOR Math::Proj(FXMVECTOR v, FXMVECTOR n)
{
    return XMVector3Dot(v, n) / XMVector3LengthSq(n) * n;
    
    XMVECTOR proj;
    XMVector3ComponentsFromNormal(&proj, nullptr, v, n);
    return proj;
}

// Matrix math abstractions.
XMFLOAT4X4 Math::Identity()
{
    XMFLOAT4X4 I(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    return I;
}

// Presently for reference only. Use XMMatrixLookAtLH()
XMMATRIX Math::ViewMatrix(
    FXMVECTOR eyePos, FXMVECTOR lookDir, FXMVECTOR up)
{
    static XMVECTOR epsilon = XMVectorSet(0.001f, 0.001f, 0.001f, 0.001f);
    XMVECTOR lookDirCpy = lookDir;
    if (XMVector3NearEqual(eyePos, lookDir, epsilon))
    {
        lookDirCpy = XMVectorSetZ(lookDir, XMVectorGetZ(lookDir) + 1.f);
    }
    return XMMatrixLookAtLH(eyePos, lookDirCpy, up);
    /*XMVECTOR w = lookDir - eyePos;
    w = w / XMVector3Length(w);
    XMVECTOR u = XMVector3Cross(up, w);
    u = u / XMVector3Length(u);
    XMVECTOR v = XMVector3Cross(w, u);
    XMVECTOR invEyePos = -1 * eyePos;

    XMFLOAT4X4 viewTransform(
        XMVectorGetX(u), XMVectorGetX(v), XMVectorGetX(w), 0.0f,
        XMVectorGetY(u), XMVectorGetY(v), XMVectorGetY(w), 0.0f,
        XMVectorGetZ(u), XMVectorGetZ(v), XMVectorGetZ(w), 0.0f,
        Dot(invEyePos, u), Dot(invEyePos, v), Dot(invEyePos, w), 0.0f
    );
    return viewTransform;*/
}

XMMATRIX Math::PerspectiveMatrix(
    const float near,
    const float far,
    const float fov,
    const float aspect)
{
    return XMMatrixPerspectiveFovLH(fov, aspect, near, far);
    /*const float t = tan(fov / 2.0f);
    const float a = far / (far - near);
    const float b = -far * near / (far - near);
    XMFLOAT4X4 persp(
        oneOver(aspect*t), 0.0f, 0.0f, 0.0f,
        0.0f, oneOver(t), 0.0f, 0.0f,
        0.0f, 0.0f, a, 1.0f,
        0.0f, 0.0f, b, 0.0f
    );

    return persp;*/
}
