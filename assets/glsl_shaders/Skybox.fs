#version 410 core
out vec4 FragColor;

uniform samplerCube uEnvironmentCubemap;

#include "common/Functions.glsl"

in vec3 UVW;

void main()
{
    vec3 color = texture(uEnvironmentCubemap, UVW).rgb;
    FragColor = vec4(color, 1.0);
}