#version 410 core
out vec4 FragColor;

in vec2 UV0;

uniform sampler2D uTexture;

void main()
{
    FragColor = texture(uTexture, UV0);
}