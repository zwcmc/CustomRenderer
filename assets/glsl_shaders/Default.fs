#version 410 core

#include "pbr/Lighting.glsl"

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

uniform sampler2D uMetallicRoughnessMap;
uniform float uMetallicRoughnessMapSet;
uniform float uMetallicFactor;
uniform float uRoughnessFactor;

uniform sampler2D uOcclusionMap;
uniform float uOcclusionMapSet;

uniform float uAlphaTestSet;
uniform float uAlphaCutoff;

uniform vec3 uLightDirection;
uniform vec3 uLightColor;
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
    // vec3 B = normalize(ddyPos * ddxUV.s - ddyPos * ddyUV.s);
    vec3 B = normalize(cross(N, T));

    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

vec4 SRGBtoLINEAR(vec4 srgbIn)
{
    vec3 linOut = pow(srgbIn.xyz, vec3(2.2));
    return vec4(linOut, srgbIn.w);
}

void main()
{
    vec4 albedo = uAlbedoMapSet > 0.0 ? SRGBtoLINEAR(texture(uAlbedoMap, fs_in.UV0)) * uBaseColor : uBaseColor;

    if (uAlphaTestSet > 0.0)
    {
        if (albedo.a < uAlphaCutoff)
        {
            discard;
        }
    }

    vec3 worldNormal = uNormalMapSet > 0.0 ? getNormal() : normalize(fs_in.Normal);

    float metallic = uMetallicFactor;
    float perceptualRoughness = uRoughnessFactor;
    if (uMetallicRoughnessMapSet > 0.0)
    {
        // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
        vec4 metallicRoughness = texture(uMetallicRoughnessMap, fs_in.UV0);
        perceptualRoughness = metallicRoughness.g * perceptualRoughness;
        metallic *= metallicRoughness.b;
    }

    vec3 worldViewDir = normalize(uCameraPos.xyz - fs_in.WorldPos);
    vec3 worldLightDir = normalize(uLightDirection);

    float alphaRoughness = perceptualRoughness * perceptualRoughness;

    vec3 color = PBRLighting(albedo.rgb, worldNormal, metallic, alphaRoughness, fs_in.WorldPos, worldViewDir, worldLightDir, uLightColor);

    if (uOcclusionMapSet > 0.0)
    {
        float ao = texture(uOcclusionMap, fs_in.UV0).r;
        color *= ao;
    }

    vec3 emission = uEmissiveMapSet > 0.0 ? SRGBtoLINEAR(texture(uEmissiveMap, fs_in.UV0)).rgb * uEmissiveColor : vec3(0.0f, 0.0f, 0.0f);
    color += emission;

    // Gamma correction
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, albedo.a);
}