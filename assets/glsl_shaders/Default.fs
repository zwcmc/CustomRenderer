#version 410 core

in VertexData
{
    vec2 UV0;
    vec3 Normal;
    vec3 FragPos;
} fs_in;

uniform sampler2D uAlbedoMap;
uniform float uAlbedoMapSet;
uniform vec4 uBaseColor;

uniform vec3 uLightPos;
uniform vec4 uLightColorIntensity; // { xyz: color, w: intensity }
uniform vec3 uCameraPos;

out vec4 FragColor;

void main()
{
    vec4 color = uAlbedoMapSet > 0.0 ? texture(uAlbedoMap, fs_in.UV0) * uBaseColor : uBaseColor;

    vec3 lightColor = uLightColorIntensity.rgb * uLightColorIntensity.a;

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * color.rgb;

    vec3 normal = normalize(fs_in.Normal);
    vec3 positionWS = fs_in.FragPos;

    vec3 lightDir = normalize(uLightPos.xyz - positionWS);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * color.rgb * lightColor;

    vec3 viewDir = normalize(uCameraPos.xyz - positionWS);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}