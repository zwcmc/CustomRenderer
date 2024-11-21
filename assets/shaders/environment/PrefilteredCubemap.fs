#version 410 core
out vec4 FragColor;

in vec3 UVW;

uniform samplerCube uEnvironmentCubemap;
uniform float uRoughness;

#include "common/constants.glsl"
#include "common/functions.glsl"

void main()
{
    // assume N = V = R
    vec3 R = normalize(UVW);
    vec3 N = R;
    vec3 V = R;

    const uint NumSamples = 4096u;

    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;

    for (uint i = 0u; i < NumSamples; ++i)
    {
        vec2 Xi = Hammersley2d(i, NumSamples);

        vec3 H = ImportanceSampleGGX(Xi, N, uRoughness);

        vec3 L = normalize(2.0 * dot(V, H) * H - V); // reflect(-V, H);

        float w = max(dot(N, L), 0.0);
        if (w > 0.0)
        {
            prefilteredColor += texture(uEnvironmentCubemap, L).rgb * w;
            totalWeight += w;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;

    FragColor = vec4(vec3(prefilteredColor), 1.0);
}