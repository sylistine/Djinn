#include "Math.h"

// cmath extensions
inline float Djinn::Math::cot(const float& theta)
{
    return tan(PI_DIVIDED_BY_TWO - theta);
}

inline float Djinn::Math::oneOver(const float& a)
{
    return 1.0f / a;
}

// Vector math extensions.
inline float Dot(const XMVECTOR& a, const XMVECTOR& b)
{
    // return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    return XMVectorGetX(XMVector3Dot(a, b));
}

// Vector math abstractions.
inline float Djinn::Math::Magnitude(const XMVECTOR& vector)
{
    return 0;
}

XMVECTOR Djinn::Math::Proj(const XMVECTOR v, const XMVECTOR n)
{
    /*
    * return dot(v, n) / pow(magnitude(n), 2.0f) * n
    */
    XMVECTOR proj;
    XMVector3ComponentsFromNormal(&proj, nullptr, v, n);
    return proj;
}

// Matrix math abstractions.
XMFLOAT4X4 Djinn::Math::Identity()
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
XMFLOAT4X4 Djinn::Math::ViewTransform(
    const XMVECTOR& eyePos, const XMVECTOR& lookDir, const XMVECTOR& up)
{
    // Get the camera's axes.
    XMVECTOR w = lookDir - eyePos; // camera z-axis/forward.
    w = w / XMVector3Length(w); // normalized.
    XMVECTOR u = XMVector3Cross(up, w); // camera x-axis/right.
    u = u / XMVector3Length(u); // normalized.
    XMVECTOR v = XMVector3Cross(w, u); // camera y-axis/up normalized.
    XMVECTOR invEyePos = -1 * eyePos;

    XMFLOAT4X4 viewTransform(
        XMVectorGetX(u), XMVectorGetX(v), XMVectorGetX(w), 0.0f,
        XMVectorGetY(u), XMVectorGetY(v), XMVectorGetY(w), 0.0f,
        XMVectorGetZ(u), XMVectorGetZ(v), XMVectorGetZ(w), 0.0f,
        Dot(invEyePos, u), Dot(invEyePos, v), Dot(invEyePos, w), 0.0f
    );
    return viewTransform;
}

// Presently for reference only. Use XMMatrixPerspectiveFovLH() or ...RH()
XMFLOAT4X4 Djinn::Math::Perspective(
    const float near,
    const float far,
    const float fov,
    const float aspect)
{
    const float t = tan(fov / 2.0f);
    const float a = far / (far - near);
    const float b = -far * near / (far - near);
    XMFLOAT4X4 persp(
        oneOver(aspect*t), 0.0f, 0.0f, 0.0f,
        0.0f, oneOver(t), 0.0f, 0.0f,
        0.0f, 0.0f, a, 1.0f,
        0.0f, 0.0f, b, 0.0f
    );

    return persp;
}