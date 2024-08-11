#version 410 core
out vec4 FragColor;

#include "common/Functions.glsl"

in vec2 UV0;

uniform sampler2D uSource;

void main()
{
    vec3 color = texture(uSource, UV0).rgb;

    // HDR tonemapping
    const float exposure = 1.0;
    color *= exposure;
    color = ACES(color);

    // Gamma correction in final blit
    color = GammaCorrection(color);

    FragColor = vec4(color, 1.0);
}