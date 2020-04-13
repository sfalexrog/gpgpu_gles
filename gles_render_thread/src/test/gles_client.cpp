#include <gles_render_thread/gles_render_thread.h>
#include <GLES2/gl2.h>

#include <iostream>

#include <thread>

int main(int argc, char** argv)
{
    GlesRenderThread::init({"", "/dev/dri/renderD129:/dev/dri/renderD128", "llvmpipe:swrast", false});
    while(!GlesRenderThread::enqueue([](void*)
    {
        std::cout << " Vendor: " << glGetString(GL_VENDOR) << std::endl;
        std::cout << " Version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << " Renderer: " << glGetString(GL_RENDERER) << std::endl;
        std::cout << " Shader version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    }, nullptr)
    ) {std::this_thread::sleep_for(std::chrono::milliseconds(1000));}

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    GlesRenderThread::shutdown();
    return 0;
}
