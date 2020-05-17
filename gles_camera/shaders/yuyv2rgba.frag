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

    gl_FragColor = vec4(yuv2rgb(ycrcb), 1.0);
}
