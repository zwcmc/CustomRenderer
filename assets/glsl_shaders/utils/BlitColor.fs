#version 410 core
out vec4 FragColor;

#include "common/functions.glsl"
#include "post_processing/tonemapping.glsl"

in vec2 UV0;

uniform sampler2D uSourceTex;

void main()
{
    FragColor = texture(uSourceTex, UV0);
}