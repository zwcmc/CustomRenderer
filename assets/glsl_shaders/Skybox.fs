#version 410 core
out vec4 FragColor;

uniform samplerCube uCubemap;

in vec3 WorldPos;

void main()
{
    vec3 color = textureLod(uCubemap, WorldPos, 0.0).rgb;
    FragColor = vec4(color, 1.0);
}