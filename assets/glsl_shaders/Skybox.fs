#version 410 core
out vec4 FragColor;

#include "common/Defines.glsl"

uniform samplerCube uCubemap;

in vec3 WorldPos;

void main()
{
    vec3 color = textureLod(uCubemap, WorldPos, 0.0).rgb;

    color = pow(color, vec3(1.0 / GAMMA));

    FragColor = vec4(color, 1.0);
}