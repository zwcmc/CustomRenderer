#ifndef SHADOWS_GLSL
#define SHADOWS_GLSL

#include "common/constants.glsl"
#include "shader_library/unity_shadow_sampling_tent.glsl"

// Should match shadowmap render target size in C++ file: { 1.0 / width, 1.0 / height, width, height }
#define MAIN_SHADOWMAP_SIZE vec4(0.00048828125, 0.00048828125, 2048.0, 2048.0)
#define POISSON_SAMPLE_NUM 20
#define FIXED_DEPTH_OFFSET 5e-3

float Rand_2to1(vec2 uv)
{ 
    // 0 - 1
    const float a = 12.9898, b = 78.233, c = 43758.5453;
    float dt = dot(uv.xy, vec2(a, b)), sn = mod(dt, M_PI);
    return fract(sin(sn) * c);
}

void ClampTexCoordsToCascadeForPCF(inout vec4 shadowCoord, vec4 clampRange)
{
    shadowCoord.xy = clamp(shadowCoord.xy, clampRange.xy + MAIN_SHADOWMAP_SIZE.xy, clampRange.zw - MAIN_SHADOWMAP_SIZE.xy);
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

float SampleShadowMap(sampler2DShadow shadowmap, vec4 shadowCoord, vec4 cascadeTexCoordsClamp)
{
    ClampTexCoordsToCascadeForPCF(shadowCoord, cascadeTexCoordsClamp);

    shadowCoord.z -= FIXED_DEPTH_OFFSET;

    return texture(shadowmap, shadowCoord.xyz);
}

float SampleShadowMapPoissonDisk(sampler2DShadow shadowmap, vec4 shadowCoord)
{
    vec2 poissonUV[POISSON_SAMPLE_NUM];
    PoissonDiskSamples(shadowCoord.xy, poissonUV);

    float shadow = 0.0;
    for (int i = 0; i < POISSON_SAMPLE_NUM; ++i)
    {
        shadow += SampleShadowMap(shadowmap, vec4(shadowCoord.xy + poissonUV[i] * MAIN_SHADOWMAP_SIZE.xy, shadowCoord.zw), vec4(vec2(0.0), vec2(1.0)));
    }
    return shadow / float(POISSON_SAMPLE_NUM);
}

float SampleShadowMapPCFTent(sampler2DShadow shadowmap, vec4 shadowCoord, vec4 cascadeTexCoordsClamp)
{
    float fetchesWeights[9];
    vec2 fetchesUV[9];

    SampleShadow_ComputeSamples_Tent_5x5(MAIN_SHADOWMAP_SIZE, shadowCoord.xy, fetchesWeights, fetchesUV);

    return fetchesWeights[0] * SampleShadowMap(shadowmap, vec4(fetchesUV[0], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[1] * SampleShadowMap(shadowmap, vec4(fetchesUV[1], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[2] * SampleShadowMap(shadowmap, vec4(fetchesUV[2], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[3] * SampleShadowMap(shadowmap, vec4(fetchesUV[3], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[4] * SampleShadowMap(shadowmap, vec4(fetchesUV[4], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[5] * SampleShadowMap(shadowmap, vec4(fetchesUV[5], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[6] * SampleShadowMap(shadowmap, vec4(fetchesUV[6], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[7] * SampleShadowMap(shadowmap, vec4(fetchesUV[7], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[8] * SampleShadowMap(shadowmap, vec4(fetchesUV[8], shadowCoord.zw), cascadeTexCoordsClamp);
}

#endif