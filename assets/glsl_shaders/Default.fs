#version 410 core

in VertexData
{
    vec2 UV0;
    vec3 Normal;
    vec3 WorldPos;
} fs_in;

uniform sampler2D uAlbedoMap;
uniform float uAlbedoMapSet;
uniform vec4 uBaseColor;

uniform sampler2D uNormalMap;
uniform float uNormalMapSet;

uniform sampler2D uEmissiveMap;
uniform float uEmissiveMapSet;
uniform vec3 uEmissiveColor;

uniform vec3 uLightPos;
uniform vec4 uLightColorIntensity; // { xyz: color, w: intensity }
uniform vec3 uCameraPos;

out vec4 FragColor;

vec3 getNormal()
{
    vec3 tangentNormal = texture(uNormalMap, fs_in.UV0).xyz * 2.0 - 1.0;

    vec3 ddxPos = dFdx(fs_in.WorldPos);
    vec3 ddyPos = dFdy(fs_in.WorldPos);
    vec2 ddxUV = dFdx(fs_in.UV0);
    vec2 ddyUV = dFdy(fs_in.UV0);

    vec3 N = normalize(fs_in.Normal);
    vec3 T = normalize(ddxPos * ddyUV.t - ddyPos * ddxUV.t);
    vec3 B = normalize(ddyPos * ddxUV.s - ddyPos * ddyUV.s);

    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{
    vec4 color = uAlbedoMapSet > 0.0 ? texture(uAlbedoMap, fs_in.UV0) * uBaseColor : uBaseColor;

    vec3 lightColor = uLightColorIntensity.rgb * uLightColorIntensity.a;

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * color.rgb;

    vec3 normal = uNormalMapSet > 0.0 ? getNormal() : normalize(fs_in.Normal);
    vec3 positionWS = fs_in.WorldPos;

    vec3 lightDir = normalize(uLightPos.xyz - positionWS);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * color.rgb * lightColor;

    vec3 viewDir = normalize(uCameraPos.xyz - positionWS);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    vec3 emission = uEmissiveMapSet > 0.0 ? texture(uEmissiveMap, fs_in.UV0).rgb * uEmissiveColor : vec3(0.0f, 0.0f, 0.0f);

    FragColor = vec4(ambient + diffuse + specular + emission, 1.0);
}