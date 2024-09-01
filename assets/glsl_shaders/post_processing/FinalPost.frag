#version 410 core
out vec4 OutColor;

in vec2 UV0;

#include "common/functions.glsl"
#include "shader_library/tonemapping.glsl"
#include "post_processing/FXAA.glsl"

uniform sampler2D uSourceTex;
uniform vec4 uSourceTexelSize; // { x: 1.0 / width, y: 1.0 / height, z: width, w: height }

uniform float uFXAASet;

void main()
{
    vec2 uv = UV0;

    vec4 color = texture(uSourceTex, uv);

    if (uFXAASet > 0.0)
    {
        color = ApplyFXAA(color, uSourceTex, uSourceTexelSize, uv);
    }

    // HDR tonemapping
    color.rgb = ACESFilm(color.rgb);

    // Gamma correction in final blit
    color = GammaCorrection(color);

    OutColor = color;
}