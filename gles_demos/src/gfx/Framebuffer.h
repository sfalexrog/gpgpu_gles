#pragma once

#include <GLES3/gl3.h>

namespace Gfx
{
    struct FboData
    {
        GLuint framebuffer;
        GLuint texture;
        GLsizei width;
        GLsizei height;
    };

    /// Create a framebuffer object
    FboData createFbo(GLsizei width, GLsizei height);

    /// Destroy a framebuffer object
    void deleteFbo(FboData& fbo);
}
