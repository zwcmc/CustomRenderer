#version 410 core

in vec2 UV0;
in vec3 Normal;

uniform sampler2D uAlbedoMap;
uniform float uAlbedoMapSet;
uniform vec4 uBaseColor;

out vec4 FragColor;

void main()
{
    vec4 color = uAlbedoMapSet > 0.0 ? texture(uAlbedoMap, UV0) * uBaseColor : uBaseColor;
    FragColor = vec4(color.rgb, 1.0);
}