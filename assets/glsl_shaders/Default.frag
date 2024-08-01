#version 410 core

in vec2 UV0;
in vec3 Normal;

uniform sampler2D albedoMap;
uniform float albedoMapSet;
uniform vec4 baseColor;

out vec4 FragColor;

void main()
{
    vec4 color = albedoMapSet > 0.0 ? texture(albedoMap, UV0) * baseColor : baseColor;
    FragColor = vec4(color.rgb, 1.0);
}