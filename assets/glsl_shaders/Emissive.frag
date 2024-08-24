#version 410 core
out vec4 OutColor;

uniform vec3 uEmissiveColor;

void main()
{
    OutColor = vec4(uEmissiveColor.rgb, 1.0);
}