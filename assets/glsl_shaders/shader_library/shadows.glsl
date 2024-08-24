#ifndef SHADOWS_GLSL
#define SHADOWS_GLSL

#include "common/constants.glsl"
#include "shader_library/unity_shadow_sampling_tent.glsl"

// Should match shadowmap render target size in C++ file: { 1.0 / width, 1.0 / height, width, height }
#define MAIN_SHADOWMAP_SIZE vec4(0.00048828125, 0.00048828125, 2048.0, 2048.0)

#define POISSON_SAMPLE_NUM 20

float Rand_2to1(vec2 uv)
{ 
    // 0 - 1
    const float a = 12.9898, b = 78.233, c = 43758.5453;
    float dt = dot(uv.xy, vec2(a, b)), sn = mod(dt, M_PI);
    return fract(sin(sn) * c);
}

void PoissonDiskSamples(const in vec2 randomSeed, out vec2 poissonDisk[POISSON_SAMPLE_NUM])
{
    float angleStep = M_TAU * float(POISSON_SAMPLE_NUM * 0.5) / float(POISSON_SAMPLE_NUM);

    float angle = Rand_2to1(randomSeed) * M_TAU;
    float radius = 1.0 / float(POISSON_SAMPLE_NUM);
    float radiusStep = radius;

    for( int i = 0; i < POISSON_SAMPLE_NUM; ++i)
    {
        poissonDisk[i] = vec2(cos(angle), sin(angle)) * pow(radius, 0.75);
        radius += radiusStep;
        angle += angleStep;
    }
}

float SampleShadowmap(sampler2DShadow shadowmap, vec4 shadowCoord)
{
    return texture(shadowmap, vec3(shadowCoord.xy, shadowCoord.z));
}

float SampleShadowmapPoissonDisk(sampler2DShadow shadowmap, vec4 shadowCoord)
{
    vec2 poissonUV[POISSON_SAMPLE_NUM];
    PoissonDiskSamples(shadowCoord.xy, poissonUV);

    float shadow = 0.0;
    for (int i = 0; i < POISSON_SAMPLE_NUM; ++i)
    {
        shadow += texture(shadowmap, vec3(shadowCoord.xy + poissonUV[i] * MAIN_SHADOWMAP_SIZE.xy, shadowCoord.z));
    }
    return shadow / float(POISSON_SAMPLE_NUM);
}

float SampleShadowmapTent3x3(sampler2DShadow shadowmap, vec4 shadowCoord)
{
    float fetchesWeights[4];
    vec2 fetchesUV[4];

    SampleShadow_ComputeSamples_Tent_3x3(MAIN_SHADOWMAP_SIZE, shadowCoord.xy, fetchesWeights, fetchesUV);

    return fetchesWeights[0] * texture(shadowmap, vec3(fetchesUV[0], shadowCoord.z))
            + fetchesWeights[1] * texture(shadowmap, vec3(fetchesUV[1], shadowCoord.z))
            + fetchesWeights[2] * texture(shadowmap, vec3(fetchesUV[2], shadowCoord.z))
            + fetchesWeights[3] * texture(shadowmap, vec3(fetchesUV[3], shadowCoord.z));
}

float SampleShadowmapTent5x5(sampler2DShadow shadowmap, vec4 shadowCoord)
{
    float fetchesWeights[9];
    vec2 fetchesUV[9];

    SampleShadow_ComputeSamples_Tent_5x5(MAIN_SHADOWMAP_SIZE, shadowCoord.xy, fetchesWeights, fetchesUV);

    return fetchesWeights[0] * texture(shadowmap, vec3(fetchesUV[0], shadowCoord.z))
            + fetchesWeights[1] * texture(shadowmap, vec3(fetchesUV[1], shadowCoord.z))
            + fetchesWeights[2] * texture(shadowmap, vec3(fetchesUV[2], shadowCoord.z))
            + fetchesWeights[3] * texture(shadowmap, vec3(fetchesUV[3], shadowCoord.z))
            + fetchesWeights[4] * texture(shadowmap, vec3(fetchesUV[4], shadowCoord.z))
            + fetchesWeights[5] * texture(shadowmap, vec3(fetchesUV[5], shadowCoord.z))
            + fetchesWeights[6] * texture(shadowmap, vec3(fetchesUV[6], shadowCoord.z))
            + fetchesWeights[7] * texture(shadowmap, vec3(fetchesUV[7], shadowCoord.z))
            + fetchesWeights[8] * texture(shadowmap, vec3(fetchesUV[8], shadowCoord.z));
}

float SampleShadowmapTent7x7(sampler2DShadow shadowmap, vec4 shadowCoord)
{
    float fetchesWeights[16];
    vec2 fetchesUV[16];

    SampleShadow_ComputeSamples_Tent_7x7(MAIN_SHADOWMAP_SIZE, shadowCoord.xy, fetchesWeights, fetchesUV);

    return fetchesWeights[0] * texture(shadowmap, vec3(fetchesUV[0], shadowCoord.z))
            + fetchesWeights[1] * texture(shadowmap, vec3(fetchesUV[1], shadowCoord.z))
            + fetchesWeights[2] * texture(shadowmap, vec3(fetchesUV[2], shadowCoord.z))
            + fetchesWeights[3] * texture(shadowmap, vec3(fetchesUV[3], shadowCoord.z))
            + fetchesWeights[4] * texture(shadowmap, vec3(fetchesUV[4], shadowCoord.z))
            + fetchesWeights[5] * texture(shadowmap, vec3(fetchesUV[5], shadowCoord.z))
            + fetchesWeights[6] * texture(shadowmap, vec3(fetchesUV[6], shadowCoord.z))
            + fetchesWeights[7] * texture(shadowmap, vec3(fetchesUV[7], shadowCoord.z))
            + fetchesWeights[8] * texture(shadowmap, vec3(fetchesUV[8], shadowCoord.z))
            + fetchesWeights[9] * texture(shadowmap, vec3(fetchesUV[9], shadowCoord.z))
            + fetchesWeights[10] * texture(shadowmap, vec3(fetchesUV[10], shadowCoord.z))
            + fetchesWeights[11] * texture(shadowmap, vec3(fetchesUV[11], shadowCoord.z))
            + fetchesWeights[12] * texture(shadowmap, vec3(fetchesUV[12], shadowCoord.z))
            + fetchesWeights[13] * texture(shadowmap, vec3(fetchesUV[13], shadowCoord.z))
            + fetchesWeights[14] * texture(shadowmap, vec3(fetchesUV[14], shadowCoord.z))
            + fetchesWeights[15] * texture(shadowmap, vec3(fetchesUV[15], shadowCoord.z));
}

#endif