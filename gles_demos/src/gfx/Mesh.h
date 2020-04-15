#ifndef GLESIMAGEPROCESSING_MESH_H
#define GLESIMAGEPROCESSING_MESH_H

#include <GLES3/gl3.h>
#include <vector>

namespace Gfx
{

struct VertexData
{
    GLfloat vpos[2];
    GLfloat texcoord[2];
};

struct Mesh
{
    std::vector<VertexData> vertices;
    std::vector<GLuint> indices;
};

struct GpuMesh
{
    GLuint vtxBuf;
    GLuint idxBuf;
    GLsizei idxCount;
};

Mesh createRegularMesh(unsigned int horizVertices, unsigned int vertVertices, float sizeX, float sizeY);

GpuMesh uploadMesh(const Mesh& mesh);

}

#endif //GLESIMAGEPROCESSING_MESH_H
