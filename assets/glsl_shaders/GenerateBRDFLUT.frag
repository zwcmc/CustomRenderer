#version 410 core
out vec4 FragColor;

in vec2 UV0;

#include "common/constants.glsl"
#include "common/functions.glsl"
#include "pbr/brdfs.glsl"

vec2 BRDF(float NdotV, float roughness)
{
    const vec3 N = vec3(0.0, 0.0, 1.0);

    vec3 V;
    V.x = sqrt(1.0 - NdotV * NdotV); // sin
    V.y = 0.0;
    V.z = NdotV;                     // cos

    const uint NumSamples = 1024u;

    vec2 LUT = vec2(0.0);
    for (uint i = 0u; i < NumSamples; ++i)
    {
        vec2 Xi = Hammersley2d(i, NumSamples);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);

        vec3 L = normalize(2.0 * dot(V, H) * H - V); // reflect(-V, H)

        // N = vec3(0.0, 0.0, 1.0), so NdotL = L.z and NdotH = H.z
        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if (NdotL > 0.0)
        {
            float a = sqr(roughness);
            float Gs = SmithG_GGX_EpicIBL(NdotV, a);
            Gs *= SmithG_GGX_EpicIBL(NdotL, a);

            float Gs_Vis = (Gs * VdotH) / (NdotH * NdotV);

            float FH = pow5(1.0 - VdotH);

            LUT += vec2((1.0 - FH) * Gs_Vis, FH * Gs_Vis);
        }

    }
    return LUT / float(NumSamples);
}

void main()
{
    FragColor = vec4(BRDF(UV0.x, UV0.y), 0.0, 1.0);
}