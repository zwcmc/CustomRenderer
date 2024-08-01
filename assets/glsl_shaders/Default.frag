#version 410 core

in vec2 UV0;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D uAlbedoMap;
uniform float uAlbedoMapSet;
uniform vec4 uBaseColor;

uniform vec4 uLightPos;
uniform vec4 uLightColorIntensity; // { xyz: color, w: intensity }

out vec4 FragColor;

void main()
{
    vec4 color = uAlbedoMapSet > 0.0 ? texture(uAlbedoMap, UV0) * uBaseColor : uBaseColor;

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * uLightColorIntensity.rgb;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(uLightPos.xyz - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * uLightColorIntensity.rgb;

    FragColor = vec4((ambient + diffuse) * uLightColorIntensity.a * color.rgb, 1.0);
}