#include "Mesh.h"

using namespace DirectX;
using namespace Djinn;

Mesh::Mesh()
{
    // dummy tri
    Vertex verts[] = {
        { 0.0f,  0.25f, 0.0f },
        { 0.25f,  -0.25f, 0.0f },
        { -0.25f, -0.25f, 0.0f },
    };
    vertexCount = sizeof(verts) / sizeof(Vertex);
    vertices = new Vertex[vertexCount];
    memcpy(vertices, verts, vertexCount * sizeof(Vertex));

    unsigned int idxes[] = {
        2, 1, 0
    };
    indexCount = sizeof(idxes) / sizeof(unsigned int);
    indices = new unsigned int[indexCount];
    memcpy(indices, idxes, indexCount * sizeof(unsigned int));
}


Mesh::Mesh(const Mesh& other) {
    memcpy(vertices, other.Vertices(), other.VertexCount() * sizeof(Vertex));
    vertexCount = other.VertexCount();
    memcpy(indices, other.Indices(), other.IndexCount() * sizeof(unsigned int));
    indexCount = other.IndexCount();
}


Mesh::~Mesh()
{
    delete[] vertices;
    delete[] indices;
}
