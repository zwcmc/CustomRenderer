#version 410 core
out vec4 FragColor;

in vec2 UV0;

#include "common/functions.glsl"
#include "common/uniforms.glsl"
#include "post_processing/tonemapping.glsl"
#include "post_processing/fxaa.glsl"

uniform sampler2D uSourceTex;
uniform vec4 uSourceTexSize; // { x: 1.0/width, y: 1.0/height, z: width, w: height }

void main()
{
    vec2 uv = UV0;

    vec4 color = texture(uSourceTex, uv);

    if (uFXAASet > 0.0)
    {
        color = ApplyFXAA(color, uSourceTex, uSourceTexSize.xy, uv);
    }

    // HDR tonemapping
    if (uToneMappingSet > 0.0)
    {
        color.rgb = ACESFilm(color.rgb);
    }

    // Gamma correction in final blit
    color = GammaCorrection(color);

    FragColor = color;
}