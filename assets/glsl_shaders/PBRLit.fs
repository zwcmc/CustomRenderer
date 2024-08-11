#version 410 core
out vec4 FragColor;

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

uniform samplerCube uIrradianceCubemap;
uniform samplerCube uPrefilteredCubemap;
uniform sampler2D uBRDFLUT;

#include "pbr/BRDF.glsl"
#include "common/Uniforms.glsl"
#include "common/Functions.glsl"

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

    vec3 worldViewDir = normalize(cameraPos - fs_in.WorldPos);
    vec3 worldLightDir = normalize(lightDirection0);

    float alphaRoughness = perceptualRoughness * perceptualRoughness;
    float roughness = alphaRoughness;
    vec3 lightColor = lightColor0;

    // vec3 color = PBRLighting(albedo.rgb, worldNormal, metallic, alphaRoughness, fs_in.WorldPos, worldViewDir, worldLightDir, lightColor0);

    vec3 N = normalize(worldNormal);
    vec3 V = normalize(worldViewDir);
    vec3 L = normalize(worldLightDir);
    vec3 H = normalize(L + V);

    vec3 F0 = vec3(0.04);

    vec3 brdfSpecular = mix(F0, albedo.rgb, metallic);

    vec3 radiance = lightColor;

    float NdotV = clamp(abs(dot(N, V)), 0.001, 1.0);
    float NdotL = clamp(dot(N, L), 0.001, 1.0);
    float HdotV = clamp(dot(H, V), 0.0, 1.0);
    float NdotH = clamp(dot(N, H), 0.0, 1.0);

    vec3 Lo = vec3(0.0);

    // BRDF
    float D = MicrofacetDistribution(NdotH, roughness);
    vec3 F = fresnelSchlick(HdotV, brdfSpecular);
    float G = GeometricOcclusion(NdotV, NdotL, roughness);
    vec3 numerator = D * G * F;
    float denominator = max((4.0 * NdotL * NdotV), 0.001);
    vec3 BRDF = numerator / denominator;
    Lo += BRDF * radiance * NdotL;

    // Diffuse
    vec3 oneMinusDielectricSpec = vec3(1.0) - F0;
    vec3 brdfDiffuse = albedo.rgb * (oneMinusDielectricSpec - metallic * oneMinusDielectricSpec);
    Lo += (brdfDiffuse / M_PI) * radiance * NdotL;

    // IBL irradiance
    Lo += texture(uIrradianceCubemap, N).rgb * brdfDiffuse;

    // IBL specular
    vec3 R = reflect(-V, N); 
    const float prefilteredCubeMipLevels = 10.0;
    vec3 prefilteredColor = textureLod(uPrefilteredCubemap, R, roughness * prefilteredCubeMipLevels).rgb;
    vec2 envBRDF = texture(uBRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    Lo += prefilteredColor * (F * envBRDF.x + envBRDF.y);

    if (uOcclusionMapSet > 0.0)
    {
        float ao = texture(uOcclusionMap, fs_in.UV0).r;
        Lo *= ao;
    }

    vec3 emission = uEmissiveMapSet > 0.0 ? SRGBtoLINEAR(texture(uEmissiveMap, fs_in.UV0)).rgb * uEmissiveColor : vec3(0.0, 0.0, 0.0);
    Lo += emission;

    FragColor = vec4(Lo, albedo.a);
}