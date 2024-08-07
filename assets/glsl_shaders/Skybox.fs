#version 410 core
out vec4 FragColor;

#include "common/Functions.glsl"

uniform samplerCube uCubemap;

in vec3 WorldPos;

void main()
{
    vec3 color = SRGBtoLINEAR(textureLod(uCubemap, WorldPos, 0.0)).rgb;
    color = GammaCorrection(color);
    FragColor = vec4(color, 1.0);
}