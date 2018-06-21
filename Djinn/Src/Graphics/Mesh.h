#pragma once

#include <vector>
#include <DirectXMath.h>

#include "../Vertex.h"

namespace Djinn {
    /// This class will eventually be used to serialize 3d models after import.
    /// Presently only instantiates as a cube.
    class Mesh
    {
    public:
        Mesh();
        ~Mesh();
        const Vertex *Vertices()const { return vertices; }
        unsigned int VertexCount()const { return vertexCount; }
        const unsigned int *Indices()const { return indices; }
        unsigned int IndexCount()const { return indexCount; }
    private:
        Mesh(const Mesh& other);

        Vertex *vertices;
        unsigned int vertexCount;
        unsigned int *indices;
        unsigned int indexCount;
    };
}
