#version 410 core

out vec4 FragColor;

uniform vec3 uEmissiveColor;

void main()
{
    FragColor = vec4(uEmissiveColor.rgb, 1.0);
}