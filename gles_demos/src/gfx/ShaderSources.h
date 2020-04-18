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


const char sobelVertexShader[] = 
"#version 100\n"
"attribute vec2 pos;\n"
"attribute vec2 texCoord;\n"
"\n"
"uniform vec2 texelSize;\n"
"\n"
"varying vec2 tc1;\n"
"varying vec2 tc2;\n"
"varying vec2 tc3;\n"
"varying vec2 tc4;\n"
"varying vec2 tc5;\n"
"varying vec2 tc6;\n"
"varying vec2 tc7;\n"
"varying vec2 tc8;\n"
"\n"
"void main()\n"
"{\n"
"  tc1 = texCoord + vec2(-texelSize.x,  texelSize.y);\n"
"  tc2 = texCoord + vec2(-texelSize.x,  0.0);\n"
"  tc3 = texCoord + vec2(-texelSize.x, -texelSize.y);\n"
"  tc4 = texCoord + vec2(0.0,  texelSize.y);\n"
"  tc5 = texCoord + vec2(0.0, -texelSize.y);\n"
"  tc6 = texCoord + vec2(texelSize.x,  texelSize.y);\n"
"  tc7 = texCoord + vec2(texelSize.x,  0.0);\n"
"  tc8 = texCoord + vec2(texelSize.x, -texelSize.y);\n"
"  gl_Position = vec4(pos, 0.0, 1.0);\n"
"}\n";

/*
 *  +----+----+----+ 
 *  |tc1 |tc4 |tc6 |
 *  +----+----+----+
 *  |tc2 |    |tc7 |
 *  +----+----+----+
 *  |tc3 |tc5 |tc8 |
 *  +----+----+----+
 * 
 **/

const char sobelFragmentShader[] = 
"#version 100\n"
"precision mediump float;\n"
"\n"
"varying vec2 tc1;\n"
"varying vec2 tc2;\n"
"varying vec2 tc3;\n"
"varying vec2 tc4;\n"
"varying vec2 tc5;\n"
"varying vec2 tc6;\n"
"varying vec2 tc7;\n"
"varying vec2 tc8;\n"
"\n"
"uniform sampler2D tex;\n"
"\n"
"float luma(vec4 rgb)\n"
"{\n"
"  return dot(rgb, vec4(0.2126, 0.7152, 0.0722, 0.0));\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"  vec4 c1 = texture2D(tex, tc1);\n"
"  vec4 c2 = texture2D(tex, tc2);\n"
"  vec4 c3 = texture2D(tex, tc3);\n"
"  vec4 c4 = texture2D(tex, tc4);\n"
"  vec4 c5 = texture2D(tex, tc5);\n"
"  vec4 c6 = texture2D(tex, tc6);\n"
"  vec4 c7 = texture2D(tex, tc7);\n"
"  vec4 c8 = texture2D(tex, tc8);\n"
"\n"
"  float vertical = luma(c1) + 2.0 * luma(c4) + luma(c6) - luma(c3) - 2.0 * luma(c5) - luma(c8);\n"
"  float horizontal = -luma(c1) - 2.0 * luma(c2) - luma(c3) + luma(c6) + 2.0 * luma(c7) + luma(c8);\n"
"  float magnitude = sqrt(vertical * vertical + horizontal * horizontal);"
"  gl_FragColor = vec4(vertical, horizontal, magnitude, 1.0);\n"
"}\n"
"\n";

}
}
