#ifndef SHADOWS_GLSL
#define SHADOWS_GLSL

#include "common/constants.glsl"
#include "common/uniforms.glsl"
#include "shader_library/unity_shadow_sampling_tent.glsl"

#define POISSON_SAMPLE_NUM 20
#define FIXED_DEPTH_OFFSET 5e-3

const vec4 CascadeColors[4] = vec4[4]
(
    vec4(1.0, 0.0, 0.0, 1.0),
    vec4(0.0, 1.0, 0.0, 1.0),
    vec4(0.0, 0.0, 1.0, 1.0),
    vec4(1.0, 0.0, 1.0, 1.0)
);

float Rand_2to1(vec2 uv)
{ 
    // 0 - 1
    const float a = 12.9898, b = 78.233, c = 43758.5453;
    float dt = dot(uv.xy, vec2(a, b)), sn = mod(dt, M_PI);
    return fract(sin(sn) * c);
}

int GetCascadeIndex(in vec4 texShadowView)
{
    int currentCascadeIndex = 0;
    int cascadeCnt = int(CascadeParams.x);
    if (cascadeCnt > 0)
    {
        int cascadeFound = 0;
        vec4 shadowCoord;
        for (int cascadeIndex = 0; cascadeIndex < cascadeCnt && cascadeFound == 0; ++cascadeIndex)
        {
            shadowCoord = ShadowProjections[cascadeIndex] * texShadowView;
            // Perspective division
            shadowCoord.xyz /= shadowCoord.w;
            if (min(shadowCoord.x, shadowCoord.y) > CascadeParams.y && max(shadowCoord.x, shadowCoord.y) < CascadeParams.z)
            {
                currentCascadeIndex = cascadeIndex;
                cascadeFound = 1;
            }
        }
    }

    return currentCascadeIndex;
}

void CalculateShadowCoordAndCascadeClampedTex(in vec4 texShadowView, out vec4 shadowCoord, out vec4 cascadeClampedTexcoord)
{
    int cascadeCnt = int(CascadeParams.x);
    int currentCascadeIndex = 0;
    if (cascadeCnt > 1)
    {
        int cascadeFound = 0;
        for (int cascadeIndex = 0; cascadeIndex < cascadeCnt && cascadeFound == 0; ++cascadeIndex)
        {
            shadowCoord = ShadowProjections[cascadeIndex] * texShadowView;
            // Perspective division
            shadowCoord.xyz /= shadowCoord.w;
            if (min(shadowCoord.x, shadowCoord.y) > CascadeParams.y && max(shadowCoord.x, shadowCoord.y) < CascadeParams.z)
            {
                currentCascadeIndex = cascadeIndex;
                cascadeFound = 1;
            }
        }
        shadowCoord.xy = shadowCoord.xy * CascadeScalesAndOffsets[currentCascadeIndex].xy + CascadeScalesAndOffsets[currentCascadeIndex].zw;
    }
    else
    {
        shadowCoord = ShadowProjections[currentCascadeIndex] * texShadowView;
        // Perspective division
        shadowCoord.xyz /= shadowCoord.w;
    }

    cascadeClampedTexcoord.xy = CascadeScalesAndOffsets[currentCascadeIndex].zw;
    cascadeClampedTexcoord.zw = cascadeClampedTexcoord.xy + 0.5;
    cascadeClampedTexcoord.xy += ShadowMapTexelSize.xy;
    cascadeClampedTexcoord.zw -= ShadowMapTexelSize.xy;
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

float SampleShadowMap(sampler2DShadow shadowmap, vec4 shadowCoord, vec4 clampedTexcoord)
{
    // Clamp texcoord in the current cascade
    shadowCoord.xy = clamp(shadowCoord.xy, clampedTexcoord.xy, clampedTexcoord.zw);

    // Fixed depth offset
    shadowCoord.z -= FIXED_DEPTH_OFFSET;

    return texture(shadowmap, shadowCoord.xyz);
}

float SampleShadowMapBilinearPCF(sampler2DShadow shadowmap, vec4 texShadowView)
{
    vec4 shadowCoord;
    vec4 clampedCascadeTexcoord;
    CalculateShadowCoordAndCascadeClampedTex(texShadowView, shadowCoord, clampedCascadeTexcoord);

    return SampleShadowMap(shadowmap, shadowCoord, clampedCascadeTexcoord);
}

float SampleShadowMapPoissonDisk(sampler2DShadow shadowmap, vec4 texShadowView)
{
    vec4 shadowCoord;
    vec4 clampedCascadeTexcoord;
    CalculateShadowCoordAndCascadeClampedTex(texShadowView, shadowCoord, clampedCascadeTexcoord);

    vec2 poissonUV[POISSON_SAMPLE_NUM];
    PoissonDiskSamples(shadowCoord.xy, poissonUV);

    float shadow = 0.0;
    for (int i = 0; i < POISSON_SAMPLE_NUM; ++i)
    {
        shadow += SampleShadowMap(shadowmap, vec4(shadowCoord.xy + poissonUV[i] * ShadowMapTexelSize.xy, shadowCoord.zw), vec4(vec2(0.0), vec2(1.0)));
    }
    return shadow / float(POISSON_SAMPLE_NUM);
}

float SampleShadowMapPCFTent(sampler2DShadow shadowmap, vec4 texShadowView)
{
    vec4 shadowCoord;
    vec4 clampedCascadeTexcoord;
    CalculateShadowCoordAndCascadeClampedTex(texShadowView, shadowCoord, clampedCascadeTexcoord);

    float fetchesWeights[9];
    vec2 fetchesUV[9];

    SampleShadow_ComputeSamples_Tent_5x5(ShadowMapTexelSize, shadowCoord.xy, fetchesWeights, fetchesUV);

    return fetchesWeights[0] * SampleShadowMap(shadowmap, vec4(fetchesUV[0], shadowCoord.zw), clampedCascadeTexcoord)
            + fetchesWeights[1] * SampleShadowMap(shadowmap, vec4(fetchesUV[1], shadowCoord.zw), clampedCascadeTexcoord)
            + fetchesWeights[2] * SampleShadowMap(shadowmap, vec4(fetchesUV[2], shadowCoord.zw), clampedCascadeTexcoord)
            + fetchesWeights[3] * SampleShadowMap(shadowmap, vec4(fetchesUV[3], shadowCoord.zw), clampedCascadeTexcoord)
            + fetchesWeights[4] * SampleShadowMap(shadowmap, vec4(fetchesUV[4], shadowCoord.zw), clampedCascadeTexcoord)
            + fetchesWeights[5] * SampleShadowMap(shadowmap, vec4(fetchesUV[5], shadowCoord.zw), clampedCascadeTexcoord)
            + fetchesWeights[6] * SampleShadowMap(shadowmap, vec4(fetchesUV[6], shadowCoord.zw), clampedCascadeTexcoord)
            + fetchesWeights[7] * SampleShadowMap(shadowmap, vec4(fetchesUV[7], shadowCoord.zw), clampedCascadeTexcoord)
            + fetchesWeights[8] * SampleShadowMap(shadowmap, vec4(fetchesUV[8], shadowCoord.zw), clampedCascadeTexcoord);
}

#endif