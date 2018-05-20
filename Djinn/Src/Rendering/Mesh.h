#pragma once

#include <vector>
#include <DirectXMath.h>

class Mesh
{
public:
    Mesh();
    Mesh(const Mesh& other);
    ~Mesh();
private:
    DirectX::XMFLOAT4 *verts;
    size_t vertc;
    int *tris;
    size_t tric;
};

