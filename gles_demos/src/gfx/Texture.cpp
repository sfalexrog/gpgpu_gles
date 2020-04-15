#include "Texture.h"
#include <sensor_msgs/image_encodings.h>

namespace Gfx
{

TextureData createTexture(int width, int height, void* data, GLenum format)
{
    TextureData td;
    glGenTextures(1, &td.id);
    glBindTexture(GL_TEXTURE_2D, td.id);
    td.width = width;
    td.height = height;
    glTexImage2D(GL_TEXTURE_2D, 0, format, td.width, td.height, 0, format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return td;
}

TextureData updateTexture(TextureData &td, int width, int height, void* data, GLenum format)
{
    glBindTexture(GL_TEXTURE_2D, td.id);
    if (width == td.width && height == td.height)
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, td.width, td.height, format, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        td.width = width;
        td.height = height;
        glTexImage2D(GL_TEXTURE_2D, 0, format, td.width, td.height, 0, format, GL_UNSIGNED_BYTE, data);
    }
    return td;
}

void deleteTexture(TextureData &td)
{
    glDeleteTextures(1, &td.id);
    td.width = td.height = 0;
    td.id = 0;
}

GLenum glTexFormat(const std::string& format)
{
    using namespace sensor_msgs::image_encodings;
    if (format == RGB8) return GL_RGB;
    if (format == RGBA8) return GL_RGBA;
    if (format == MONO8) return GL_LUMINANCE;
    if (format == BGR8) return GL_RGB; // FIXME: support BGR textures?
    if (format == BGRA8) return GL_RGBA; // (or at least signal somehow they should be swizzled)
    if (format == TYPE_8UC1) return GL_LUMINANCE;
    if (format == TYPE_8UC2) return GL_LUMINANCE_ALPHA;
    if (format == TYPE_8UC3) return GL_RGB;
    if (format == TYPE_8UC4) return GL_RGBA;
    return 0;
}

}
