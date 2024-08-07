#version 410 core
out vec4 FragColor;

#include "common/Functions.glsl"

in vec2 UV0;

uniform sampler2D uTexture;

void main()
{
    vec4 color = SRGBtoLINEAR(texture(uTexture, UV0));
    color.rgb = GammaCorrection(color.rgb);
    FragColor = color;
}