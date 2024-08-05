#ifndef BRDF_GLSL
#define BRDF_GLSL

#include "common/Defines.glsl"

vec3 SpecularReflection(float HdotV, vec3 F0)
{
    float x = 1.0 - HdotV; // sub
    float x2 = x * x; // mul
    float x5 = x * x2 * x2; // mul  mul
    return (1.0 - F0) * x5 + F0; // sub  mad
}

float MicrofacetDistribution(float NdotH, float roughness)
{
    float roughnessSq = roughness * roughness;
    float f = (NdotH * roughnessSq - NdotH) * NdotH + 1.0;
    return roughnessSq / (M_PI * f * f);
}

float GeometricOcclusion(float NdotV, float NdotL, float r)
{
    float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
    float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
    return attenuationL * attenuationV;
}

#endif