#version 410 core
out vec4 OutColor;

uniform samplerCube uEnvironmentCubemap;

#include "common/functions.glsl"

in vec3 UVW;

void main()
{
    vec3 color = texture(uEnvironmentCubemap, UVW).rgb;
    OutColor = vec4(color, 1.0);
}