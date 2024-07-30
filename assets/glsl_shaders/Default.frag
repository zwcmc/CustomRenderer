#version 410 core

in vec2 uv;

uniform sampler2D texture1;

out vec4 FragColor;

void main()
{
    vec4 color = texture(texture1, uv);
    FragColor = vec4(color.rgb, 1.0);
}