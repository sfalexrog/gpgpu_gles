#pragma once

namespace Gfx
{
namespace Shaders
{
const char undistortVertexShader[] = 
"#version 300 es\n"
"layout(location=0) in vec2 pos;\n"
"layout(location=1) in vec2 texCoord;\n"
"\n"
"out vec2 vTexCoord;\n"
"\n"
"void main()\n"
"{\n"
"  vTexCoord = texCoord;\n"
"  gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);\n"
"}\n";

// FIXME: This shader assumes the input texture has BGR format
const char undistortFragmentShader[] = 
"#version 300 es\n"
"precision mediump float;\n"
"uniform sampler2D tex;\n"
"\n"
"in vec2 vTexCoord;\n"
"\n"
"out vec4 Color;\n"
"\n"
"void main()\n"
"{\n"
"  Color = texture(tex, vTexCoord).bgra;\n"
"}\n";

}
}
