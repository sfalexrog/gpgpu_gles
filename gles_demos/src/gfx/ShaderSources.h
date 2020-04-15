#pragma once

namespace Gfx
{
namespace Shaders
{
const char undistortVertexShader[] = 
"#version 100\n"
"attribute vec2 pos;\n"
"attribute vec2 texCoord;\n"
"\n"
"varying vec2 vTexCoord;\n"
"\n"
"void main()\n"
"{\n"
"  vTexCoord = texCoord;\n"
"  gl_Position = vec4(pos, 0.0, 1.0);\n"
"}\n";

// FIXME: This shader assumes the input texture has BGR format
const char undistortFragmentShader[] = 
"#version 100\n"
"precision mediump float;\n"
"uniform sampler2D tex;\n"
"\n"
"varying vec2 vTexCoord;\n"
"\n"
"void main()\n"
"{\n"
"  gl_FragColor = texture2D(tex, vTexCoord).bgra;\n"
"}\n";

}
}
