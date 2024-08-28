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
    return texture(shadowmap, shadowCoord.xyz);
}

float SampleShadowMapPoissonDisk(sampler2DShadow shadowmap, vec4 shadowCoord)
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

float SampleShadowMapTent3x3(sampler2DShadow shadowmap, vec4 shadowCoord, vec4 cascadeTexCoordsClamp)
{
    float fetchesWeights[4];
    vec2 fetchesUV[4];

    SampleShadow_ComputeSamples_Tent_3x3(MAIN_SHADOWMAP_SIZE, shadowCoord.xy, fetchesWeights, fetchesUV);

    return fetchesWeights[0] * SampleShadowMap(shadowmap, vec4(fetchesUV[0], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[1] * SampleShadowMap(shadowmap, vec4(fetchesUV[1], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[2] * SampleShadowMap(shadowmap, vec4(fetchesUV[2], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[3] * SampleShadowMap(shadowmap, vec4(fetchesUV[3], shadowCoord.zw), cascadeTexCoordsClamp);
}

float SampleShadowMapTent5x5(sampler2DShadow shadowmap, vec4 shadowCoord, vec4 cascadeTexCoordsClamp)
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

float SampleShadowMapTent7x7(sampler2DShadow shadowmap, vec4 shadowCoord, vec4 cascadeTexCoordsClamp)
{
    float fetchesWeights[16];
    vec2 fetchesUV[16];

    SampleShadow_ComputeSamples_Tent_7x7(MAIN_SHADOWMAP_SIZE, shadowCoord.xy, fetchesWeights, fetchesUV);

    return fetchesWeights[0] * SampleShadowMap(shadowmap, vec4(fetchesUV[0], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[1] * SampleShadowMap(shadowmap, vec4(fetchesUV[1], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[2] * SampleShadowMap(shadowmap, vec4(fetchesUV[2], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[3] * SampleShadowMap(shadowmap, vec4(fetchesUV[3], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[4] * SampleShadowMap(shadowmap, vec4(fetchesUV[4], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[5] * SampleShadowMap(shadowmap, vec4(fetchesUV[5], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[6] * SampleShadowMap(shadowmap, vec4(fetchesUV[6], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[7] * SampleShadowMap(shadowmap, vec4(fetchesUV[7], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[8] * SampleShadowMap(shadowmap, vec4(fetchesUV[8], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[9] * SampleShadowMap(shadowmap, vec4(fetchesUV[9], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[10] * SampleShadowMap(shadowmap, vec4(fetchesUV[10], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[11] * SampleShadowMap(shadowmap, vec4(fetchesUV[11], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[12] * SampleShadowMap(shadowmap, vec4(fetchesUV[12], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[13] * SampleShadowMap(shadowmap, vec4(fetchesUV[13], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[14] * SampleShadowMap(shadowmap, vec4(fetchesUV[14], shadowCoord.zw), cascadeTexCoordsClamp)
            + fetchesWeights[15] * SampleShadowMap(shadowmap, vec4(fetchesUV[15], shadowCoord.zw), cascadeTexCoordsClamp);
}

#endif