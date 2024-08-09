#version 410 core
out vec4 FragColor;

uniform samplerCube uCubemap;

in vec3 WorldPos;

void main()
{
    vec3 color = texture(uCubemap, WorldPos).rgb;
    FragColor = vec4(color, 1.0);
}