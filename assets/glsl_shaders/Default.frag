#version 410 core

in vec2 uv;

uniform sampler2D albedoMap;
uniform float albedoMapSet;
uniform vec4 baseColor;

out vec4 FragColor;

void main()
{
    vec4 color = albedoMapSet > 0.0 ? texture(albedoMap, uv) * baseColor : baseColor;
    FragColor = vec4(color.rgb, 1.0);
}