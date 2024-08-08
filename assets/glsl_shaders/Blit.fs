#version 410 core
out vec4 FragColor;

#include "common/Functions.glsl"

in vec2 UV0;

uniform sampler2D ColorAttachment0;

void main()
{
    vec3 color = texture(ColorAttachment0, UV0).rgb;

    // Gamma correction in final blit
    color = GammaCorrection(color);

    FragColor = vec4(color, 1.0);
}