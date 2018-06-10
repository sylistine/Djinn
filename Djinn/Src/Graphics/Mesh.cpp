#include "Mesh.h"

using namespace DirectX;
using namespace Djinn;

Mesh::Mesh()
{
    // all mesh are cube
    XMFLOAT4 vertices[] = {
        XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f),
        XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),
        XMFLOAT4(1.0f, -1.0f, -1.0f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, -1.0f, 1.0f),
        XMFLOAT4(1.0f, -1.0f, 1.0f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        XMFLOAT4(-1.0f, -1.0f, 1.0f, 1.0f),
        XMFLOAT4(-1.0f, 1.0f, 1.0f, 1.0f)
    };
    verts = vertices;
    vertc = 8;
    int triangles[] = {
        0, 1, 2, 3, 2, 1,
        2, 3, 4, 5, 4, 3,
        4, 5, 6, 7, 6, 5,
        6, 7, 0, 1, 0, 7,
        0, 2, 4, 4, 6, 0,
        1, 3, 5, 5, 7, 1
    };
    tris = triangles;
    tric = 36;
}


Mesh::Mesh(const Mesh& other) {
    verts = other.verts;
    vertc = other.vertc;
    tris = other.tris;
    tric = other.tric;
}


Mesh::~Mesh()
{
    delete[] verts;
    delete[] tris;
}
