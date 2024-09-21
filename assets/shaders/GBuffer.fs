#version 410 core

layout (location = 0) out vec4 GBuffer0;
layout (location = 1) out vec4 GBuffer1;
layout (location = 2) out vec4 GBuffer2;
layout (location = 3) out vec4 GBuffer3;

in GBufferVertexData
{
    vec2 UV0;
    vec3 WorldNormal;
    vec4 WorldTangent; // xyz: tangent, w: the sign of the bitangent
    vec3 WorldPosition;
} gbuffer_fs_in;
 
// Albedo
uniform sampler2D uBaseMap;
uniform float uBaseMapSet;
uniform vec4 uBaseColor;

// Normal
uniform sampler2D uNormalMap;
uniform float uNormalMapSet;

// Emission
uniform sampler2D uEmissiveMap;
uniform float uEmissiveMapSet;
uniform vec4 uEmissiveColor;

// Metallic and roughness
uniform sampler2D uMetallicRoughnessMap;
uniform float uMetallicRoughnessMapSet;
uniform float uMetallicFactor;
uniform float uRoughnessFactor;

// Occlusion
uniform sampler2D uOcclusionMap;
uniform float uOcclusionMapSet;

// Alpha blend
uniform float uAlphaBlendSet;
// Alpha test
uniform float uAlphaTestSet;
uniform float uAlphaCutoff;

#include "common/functions.glsl"

void main()
{
    vec2 uv = gbuffer_fs_in.UV0;

    vec4 albedo = uBaseMapSet > 0.0 ? SRGBtoLINEAR(texture(uBaseMap, uv)) * uBaseColor : uBaseColor;

    // Alpha test
    if (uAlphaTestSet > 0.0)
    {
        if (albedo.a < uAlphaCutoff)
        {
            discard;
        }
    }

    float metallic = uMetallicFactor;
    float perceptualRoughness = uRoughnessFactor;
    if (uMetallicRoughnessMapSet > 0.0)
    {
        // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
        vec4 metallicRoughness = texture(uMetallicRoughnessMap, uv);
        metallic *= metallicRoughness.b;
        perceptualRoughness *= metallicRoughness.g;
    }

    vec3 N;
    if (uNormalMapSet > 0.0)
    {
        vec3 n = gbuffer_fs_in.WorldNormal;
        vec3 t = gbuffer_fs_in.WorldTangent.xyz;
        vec3 b = cross(n, t) * sign(gbuffer_fs_in.WorldTangent.w);
        mat3 TBN = mat3(t, b, n);
        vec3 tangentNormal = texture(uNormalMap, uv).xyz * 2.0 - 1.0;
        N = normalize(TBN * tangentNormal);
    }
    else
    {
        N = normalize(gbuffer_fs_in.WorldNormal);
    }

    // Occlusion
    float occlusion = 1.0;
    if (uOcclusionMapSet > 0.0)
    {
        occlusion = texture(uOcclusionMap, uv).r;
    }

    // Emissive
    vec3 emission = uEmissiveMapSet > 0.0 ? SRGBtoLINEAR(texture(uEmissiveMap, uv)).rgb * uEmissiveColor.rgb : vec3(0.0);

    GBuffer0 = vec4(albedo.rgb, metallic);                        // rgb: albedo, a: occlusion
    GBuffer1 = vec4(N, perceptualRoughness);                      // rgb: normal, a: roughness
    GBuffer2 = vec4(gbuffer_fs_in.WorldPosition, occlusion);      // rgb: world position, a: occlusion
    GBuffer3 = vec4(emission, 1.0);                               // rgb: emission, a: unused
}