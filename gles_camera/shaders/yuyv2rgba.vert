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
