#version 410 core
out vec4 OutColor;

#include "common/functions.glsl"
#include "post_processing/tonemapping.glsl"

in vec2 UV0;

uniform sampler2D uSourceTex;

void main()
{
    vec4 color = texture(uSourceTex, UV0);

    // HDR tonemapping
    color.rgb = ACESFilm(color.rgb);

    // Gamma correction in final blit
    color = GammaCorrection(color);

    OutColor = color;
}