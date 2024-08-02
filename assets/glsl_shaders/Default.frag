#version 410 core

in vec2 UV0;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D uAlbedoMap;
uniform float uAlbedoMapSet;
uniform vec4 uBaseColor;

uniform vec4 uLightPos;
uniform vec4 uLightColorIntensity; // { xyz: color, w: intensity }
uniform vec4 uCameraPos;

out vec4 FragColor;

void main()
{
    vec4 color = uAlbedoMapSet > 0.0 ? texture(uAlbedoMap, UV0) * uBaseColor : uBaseColor;

    vec3 lightColor = uLightColorIntensity.rgb * uLightColorIntensity.a;

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * color.rgb;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(uLightPos.xyz - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * color.rgb * lightColor;

    vec3 viewDir = normalize(uCameraPos.xyz - FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}