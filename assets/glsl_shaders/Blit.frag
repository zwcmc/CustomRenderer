#version 410 core
out vec4 FragColor;

#include "common/functions.glsl"
#include "shader_library/tonemapping.glsl"

in vec2 UV0;

uniform sampler2D uSource;

void main()
{
    vec4 color = texture(uSource, UV0);

    // HDR tonemapping
    color.rgb = NeutralTonemapping(color.rgb);

    // Gamma correction in final blit
    color = GammaCorrection(color);

    FragColor = color;
}