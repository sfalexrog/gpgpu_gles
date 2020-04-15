#ifndef GLESIMAGEPROCESSING_TEXTURE_H
#define GLESIMAGEPROCESSING_TEXTURE_H

#include <GLES3/gl3.h>
#include <string>

namespace Gfx
{

struct TextureData
{
    GLuint id;
    GLsizei width, height;
};

TextureData createTexture(int width, int height, void* data, GLenum format = GL_RGBA);

TextureData updateTexture(TextureData& td, int width, int height, void* data, GLenum format = GL_RGBA);

void deleteTexture(TextureData &td);

GLenum glTexFormat(const std::string& format);

}

#endif //GLESIMAGEPROCESSING_TEXTURE_H
