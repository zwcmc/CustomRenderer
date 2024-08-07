#ifndef LIGHTING_GLSL
#define LIGHTING_GLSL

#include "common/BRDF.glsl"

vec3 PBRLighting(vec3 albedo, vec3 worldNormal, float metallic, float roughness, vec3 worldPos, vec3 worldViewDir, vec3 worldLightDir, vec3 lightColor)
{
    vec3 N = normalize(worldNormal);
    vec3 V = normalize(worldViewDir);
    vec3 L = normalize(worldLightDir);
    vec3 H = normalize(L + V);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 radiance = lightColor;

    float NdotV = clamp(abs(dot(N, V)), 0.001, 1.0);
    float NdotL = clamp(dot(N, L), 0.001, 1.0);
    float HdotV = clamp(dot(H, V), 0.0, 1.0);
    float NdotH = clamp(dot(N, H), 0.0, 1.0);

    vec3 Lo = vec3(0.0);

    float D = MicrofacetDistribution(NdotH, roughness);
    vec3 F = fresnelSchlick(HdotV, F0);
    float G = GeometricOcclusion(NdotV, NdotL, roughness);

    // // cook-torrance brdf
    // float D = DistributionGGX(N, H, roughness);
    // float G = GeometrySmith(N, V, L, roughness);
    // vec3 F = fresnelSchlick(HdotV, F0);

    vec3 numerator = D * G * F;
    float denominator = max((4.0 * NdotL * NdotV), 0.001);
    vec3 BRDF = numerator / denominator;

    Lo += BRDF * radiance * NdotL;

    vec3 diffuseTerm = vec3(1.0) - F;
    diffuseTerm *= 1.0 - metallic;
    diffuseTerm = diffuseTerm * albedo / M_PI;

    Lo += diffuseTerm * radiance * NdotL;

    return Lo;
}

#endif