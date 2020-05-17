#include <SDL2/SDL.h>
#include "../v4l2device.h"
#include "../log.h"
#include <GLES3/gl3.h>

SDL_GLContext createGlContext(SDL_Window *w)
{
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    return SDL_GL_CreateContext(w);
}

v4l2_pix_format setupV4l2Device(gles_camera::V4L2Device &camera)
{
    camera.init();
    const auto& formats = camera.supportedFormats();
    const auto& sizes = camera.supportedSizes();
    int chosenFormat = -1;
    for (int format = 0; format < formats.size(); ++format)
    {
        if (!(formats[format].flags & V4L2_FMT_FLAG_COMPRESSED))
        {
            chosenFormat = format;
        }
    }
    v4l2_pix_format pf = camera.setFormat(1280, 720, formats[chosenFormat].pixelformat);
    camera.prepareBuffers(4);
    camera.startCapture();
    return pf;
}

GLuint compileShader(const char* shaderSource, GLenum shaderStage)
{
    GLuint shader = glCreateShader(shaderStage);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength);
        glGetShaderInfoLog(shader, log.size(), &logLength, log.data());
        LOG_FUNC(ERROR) << "Failed to compile shader: " << log.data();
        return 0;
    }
    return shader;
}

GLuint setupShaders()
{
    const char* vtxShader = R"vertex_shader(
#version 100

attribute vec2 aPos;
attribute vec2 aTex;

varying vec2 vPos;
varying vec2 vTex;

void main()
{
    vPos = aPos;
    vTex = aTex;
    gl_Position = vec4(vPos, 0.0, 1.0);
}
)vertex_shader";

    const char* frgShader = R"fragment_shader(
#version 100

precision mediump float;

varying vec2 vPos;
varying vec2 vTex; // Note: Must range from [0, 0] to [width-1, height-1]

uniform sampler2D tex_yuyv; // Interpreted as LUMINANCE_ALPHA
uniform vec2 texelSize;


vec3 yuv2rgb(in vec3 yuv)
{
    return vec3(
        max(0.0, min(1.0, 1.164 * (yuv.r - 0.0627) + 1.7927 * (yuv.g - 0.5))),
        max(0.0, min(1.0, 1.164 * (yuv.r - 0.0627) - 0.2132 * (yuv.g - 0.5) - 0.5329 * (yuv.b))),
        max(0.0, min(1.0, 1.164 * (yuv.r - 0.0627) + 2.1124 * (yuv.b - 0.5)))
    );
}

void main()
{
    vec3 ycrcb;
    vec2 thisTexelPos = vTex * texelSize;
    if (mod(vTex.x, 2.0) < 1.0) {
        // Even pixel coordinates: 0, 2, 4... - Cb component stored here
        vec2 nearTexelPos = thisTexelPos + vec2(texelSize.x, 0.0);
        ycrcb.rg = texture2D(tex_yuyv, thisTexelPos).ra;
        ycrcb.b = texture2D(tex_yuyv, nearTexelPos).a;
    } else {
        // Odd pixel coordinates: 1, 3, 5... - Cr component stored here
        vec2 nearTexelPos = thisTexelPos - vec2(texelSize.x, 0.0);
        ycrcb.rb = texture2D(tex_yuyv, thisTexelPos).ra;
        ycrcb.g = texture2D(tex_yuyv, nearTexelPos).a;
    }
    //gl_FragColor = vec4(texture2D(tex_yuyv, thisTexelPos).rrr, 1.0);

    gl_FragColor = vec4(yuv2rgb(ycrcb).bgr, 1.0);
    //gl_FragColor = vec4(ycrcb, 1.0);
}
)fragment_shader";
    GLuint vshader = compileShader(vtxShader, GL_VERTEX_SHADER);
    GLuint fshader = compileShader(frgShader, GL_FRAGMENT_SHADER);
    GLuint program = glCreateProgram();

    glBindAttribLocation(program, 0, "aPos");
    glBindAttribLocation(program, 1, "aTex");

    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength);
        glGetProgramInfoLog(program, log.size(), &logLength, log.data());
        LOG_FUNC(ERROR) << "Failed to compile shader: " << log.data();
        return 0;
    }
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    return program;
}

struct GlData
{
    GLfloat pos[2];
    GLfloat tex[2];
};

struct Buffers
{
    GLuint data;
    GLuint elements;
};

Buffers createBuffers(const v4l2_pix_format &fmt)
{
    GlData data[] {
        {1, -1,  0,         0},
        {1,  1,  0,         fmt.height},
        {-1, -1, fmt.width, 0},
        {-1, 1,  fmt.width, fmt.height}
    };
    GLuint elements[] = {
        0, 1, 2,
        1, 3, 2
    };

    Buffers gpuBuffers;
    glGenBuffers(1, &gpuBuffers.data);
    glGenBuffers(1, &gpuBuffers.elements);
    glBindBuffer(GL_ARRAY_BUFFER, gpuBuffers.data);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuBuffers.elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    return gpuBuffers;
}

int main(int argc, char** argv)
{
    Logger::Logger::minSeverity() = Logger::Severity::LOG_DEBUG;
    gles_camera::V4L2Device camera("/dev/video0", gles_camera::V4L2Device::CaptureMethod::Mmap);


    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("v4l2 preview window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = createGlContext(window);
    LOG_FUNC(DEBUG) << "Renderer: " << glGetString(GL_RENDERER);
    LOG_FUNC(DEBUG) << "Version: " << glGetString(GL_VERSION);
    LOG_FUNC(DEBUG) << "Vendor: " << glGetString(GL_VENDOR);
    v4l2_pix_format pixfmt = setupV4l2Device(camera);

    auto programId = setupShaders();
    glUseProgram(programId);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, pixfmt.width, pixfmt.height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glActiveTexture(GL_TEXTURE0);

    auto texLoc = glGetUniformLocation(programId, "tex_yuyv");
    glUniform1i(texLoc, 0);

    auto texelLoc = glGetUniformLocation(programId, "texelSize");
    glUniform2f(texelLoc, 1.0f / pixfmt.width, 1.0f /pixfmt.height);

    auto buffers = createBuffers(pixfmt);

    while(true)
    {
        SDL_Event ev;
        while(SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_QUIT)
            {
                return 0;
            }
        }
        auto frame = camera.readFrame();

        glUseProgram(programId);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pixfmt.width, pixfmt.height, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, frame.data().start);

        glClear(GL_COLOR_BUFFER_BIT);
        glBindBuffer(GL_ARRAY_BUFFER, buffers.data);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GlData), (void*)offsetof(GlData, pos));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GlData), (void*)offsetof(GlData, tex));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.elements);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        SDL_GL_SwapWindow(window);
    }
    SDL_Quit();
}
