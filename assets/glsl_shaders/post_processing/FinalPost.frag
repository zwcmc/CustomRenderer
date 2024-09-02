#version 410 core
out vec4 OutColor;

in vec2 UV0;

#include "common/functions.glsl"
#include "shader_library/tonemapping.glsl"
#include "post_processing/FXAA.glsl"

uniform sampler2D uSourceTex;

uniform float uFXAASet;

void main()
{
    vec2 uv = UV0;

    vec2 size = textureSize(uSourceTex, 0);
    vec2 texelSize = vec2(1.0) / size;

    vec4 color = texture(uSourceTex, uv);

    if (uFXAASet > 0.0)
    {
        color = ApplyFXAA(color, uSourceTex, texelSize, uv);
    }

    // HDR tonemapping
    color.rgb = ACESFilm(color.rgb);

    // Gamma correction in final blit
    color = GammaCorrection(color);

    OutColor = color;
}