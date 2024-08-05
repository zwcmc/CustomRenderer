#version 410 core

out vec4 FragColor;

uniform vec3 uLightColor;

void main()
{
    FragColor = vec4(uLightColor.rgb, 1.0);
}