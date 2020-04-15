#include "Framebuffer.h"
#include <stdexcept>

namespace Gfx
{
    FboData createFbo(GLsizei width, GLsizei height)
    {
        FboData fbo{0, 0, 0, 0};
        glGenFramebuffers(1, &fbo.framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo.framebuffer);
        glGenTextures(1, &fbo.texture);
        glBindTexture(GL_TEXTURE_2D, fbo.texture);
        fbo.width = width;
        fbo.height = height;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fbo.width, fbo.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.texture, 0);
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            throw std::runtime_error("Incomplete framebuffer created - cannot use for drawing");
        }
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        return fbo;
    }

    void deleteFbo(FboData& fbo)
    {
        glDeleteFramebuffers(1, &fbo.framebuffer);
        glDeleteTextures(1, &fbo.texture);
        fbo.width = fbo.height = 0;
    }
}
