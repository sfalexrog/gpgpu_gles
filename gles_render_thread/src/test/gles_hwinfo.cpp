#include <gles_render_thread/gles_render_thread.h>
#include <GLES2/gl2.h>
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>

#include <iostream>

#include <thread>
#include <atomic>

#include "gles_constant_values.h"


struct GlConstant
{
    const char* name;
    const int value;
};

#define GLCONSTANT(cname) {#cname, GLValues::_ ## cname},

GlConstant constants[] = {
#include "gles_constants.inl"
};

int main(int argc, char** argv)
{
    std::atomic<bool> done{false};
    GlesRenderThread::init({"", "/dev/dri/renderD129:/dev/dri/renderD128", "llvmpipe:swrast", false});
    while(!GlesRenderThread::enqueue([&done](void*)
    {
        std::cout << " Vendor: " << glGetString(GL_VENDOR) << std::endl;
        std::cout << " Version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << " Renderer: " << glGetString(GL_RENDERER) << std::endl;
        std::cout << " Shader version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

        std::cout << "Current limits:" << std::endl;
        for(int i = 0; i < sizeof(constants) / sizeof(constants[0]); ++i)
        {
            std::cout << " " << constants[i].name << ":\t\t";
            GLint value = 0;
            glGetIntegerv(constants[i].value, &value);
            if (glGetError() != GL_NO_ERROR)
            {
                std::cout << "[unsupported]" << std::endl;
            }
            else
            {
                std::cout << value << std::endl;
            }
        }
        done = true;
    }, nullptr)
    ) {std::this_thread::sleep_for(std::chrono::milliseconds(100));}

    while(!done)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    GlesRenderThread::shutdown();
    return 0;
}

