#version 410 core
out vec4 FragColor;

in vec2 UV0;

#include "common/Defines.glsl"
#include "common/Functions.glsl"

// ----------------------------------------------------------------------------
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// efficient VanDerCorpus calculation.
float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}
// ----------------------------------------------------------------------------
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
    
    float phi = M_TAU * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
    
    // from spherical coordinates to cartesian coordinates - halfway vector
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
    
    // from tangent-space H vector to world-space sample vector
    vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}
// ----------------------------------------------------------------------------
// NOTE(Joey): for IBL we use a different k (see unreal course notes)
float GeometryGGXSchlickIBL(float NdotV, float roughness)
{
    float a = roughness*roughness;
    float k = a / 2.0;

    float nom    = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometryGGXIBL(float NdotV, float NdotL, float roughness)
{
    float ggx1 = GeometryGGXSchlickIBL(NdotV, roughness);
    float ggx2 = GeometryGGXSchlickIBL(NdotL, roughness);
    
    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
// TODO(Joey): get proper names of the options/functions
float GeometryGGXSchlick(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom    = NdotV;
    float denom = NdotV * (1.0 - k) + k ;
    
    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometryGGX(float NdotV, float NdotL, float roughness)
{
    float ggx2 = GeometryGGXSchlick(NdotV, roughness);
    float ggx1 = GeometryGGXSchlick(NdotL, roughness);
    
    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------

vec2 BRDF(float NdotV, float roughness)
{
    const vec3 N = vec3(0.0, 0.0, 1.0);
    vec3 V = vec3(sqrt(1.0 - NdotV * NdotV), 0.0, NdotV);

    const uint SAMPLE_COUNT = 1024u;

    vec2 LUT = vec2(0.0);
    for (uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        // float NdotL = max(L.z, 0.0);
        // float NdotH = max(H.z, 0.0);
        // float VdotH = max(dot(V, H), 0.0);
        float NdotL = max(dot(N, L), 0.0);
        float NdotH = max(dot(N, H), 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if (NdotL > 0.0)
        {
            float G = GeometryGGXIBL(NdotV, NdotL, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);
            LUT += vec2((1.0 - Fc) * G_Vis, Fc * G_Vis);
        }
    }
    return LUT / float(SAMPLE_COUNT);
}

void main()
{
    FragColor = vec4(BRDF(UV0.x, UV0.y), 0.0, 1.0);
}