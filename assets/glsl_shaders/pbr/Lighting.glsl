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

    // cook-torrance brdf
    float D = MicrofacetDistribution(NdotH, roughness);
    vec3 F = SpecularReflection(HdotV, F0);
    float G = GeometricOcclusion(NdotV, NdotL, roughness);

    vec3 diffuseTerm = vec3(1.0) - F;
    diffuseTerm *= 1.0 - metallic;
    diffuseTerm = diffuseTerm * albedo / M_PI;

    vec3 specularTerm = D * F * G / (4.0 * NdotV * NdotL + 0.00001);

    vec3 Lo = (diffuseTerm + specularTerm) * radiance * NdotL;

    return Lo;
}

#endif