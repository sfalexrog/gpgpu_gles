#include "Mesh.h"

namespace Gfx
{

Mesh createRegularMesh(unsigned int horizVertices, unsigned int vertVertices, float sizeX, float sizeY)
{
    Mesh result;
    result.vertices.reserve(horizVertices * vertVertices);
    for(int iy = 0; iy < vertVertices; ++iy)
    {
        float y = sizeY * iy / (vertVertices - 1);
        float ty = float(iy) / (float(vertVertices - 1));
        for(int ix = 0; ix < horizVertices; ++ix)
        {
            float x = sizeX * ix / (horizVertices - 1);
            float tx = float(ix) / (float(horizVertices - 1));
            VertexData vertex{x, y, tx, ty};
            result.vertices.push_back(vertex);
        }
    }

    for(int iy = 0; iy < vertVertices - 1; ++iy)
    {
        for(int ix = 0; ix < horizVertices - 1; ++ix)
        {
            result.indices.push_back(horizVertices * iy + ix);
            result.indices.push_back(horizVertices * iy + ix + 1);
            result.indices.push_back(horizVertices * (iy + 1) + ix);

            result.indices.push_back(horizVertices * iy + ix + 1);
            result.indices.push_back(horizVertices * (iy + 1) + ix);
            result.indices.push_back(horizVertices * (iy + 1) + ix + 1);
        }
    }

    return result;
}

GpuMesh uploadMesh(const Mesh& mesh)
{
    GpuMesh result{};
    glGenBuffers(1, &result.vtxBuf);
    glGenBuffers(1, &result.idxBuf);

    glBindBuffer(GL_ARRAY_BUFFER, result.vtxBuf);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(VertexData), mesh.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.idxBuf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(GLuint), mesh.indices.data(), GL_STATIC_DRAW);

    result.idxCount = mesh.indices.size();

    return result;
}

}
