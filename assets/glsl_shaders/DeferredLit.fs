#version 410 core
out vec4 FragColor;

in vec2 UV0;

uniform sampler2D uGBuffer0;
uniform sampler2D uGBuffer1;
uniform sampler2D uGBuffer2;
uniform sampler2D uGBuffer3;

// IBL
uniform samplerCube uIrradianceCubemap;
uniform samplerCube uPrefilteredCubemap;
uniform sampler2D uBRDFLUT;

// Shadow
uniform sampler2DShadow uShadowMap;
uniform float uShadowMapSet;

#include "pbr/brdfs.glsl"
#include "common/uniforms.glsl"
#include "common/functions.glsl"
#include "shadows/shadows.glsl"

void main()
{
    vec2 uv = UV0;

    vec4 albedoMetallic = texture(uGBuffer0, uv);
    vec4 normalRoughness = texture(uGBuffer1, uv);
    vec4 worldPositionOcclusion = texture(uGBuffer2, uv);
    vec3 emission = texture(uGBuffer3, uv).xyz;

    vec4 albedo = vec4(albedoMetallic.xyz, 1.0);

    float metallic = albedoMetallic.w;
    float perceptualRoughness = normalRoughness.w;

    vec3 worldPosition = worldPositionOcclusion.xyz;
    float occlusion = worldPositionOcclusion.w;

    vec3 N = normalize(normalRoughness.xyz);
    vec3 V = normalize(CameraPosition - worldPosition);
    vec3 L = normalize(MainLightDirection);
    vec3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float LdotH = max(dot(L, H), 0.0);

    // Main light shadow
    float shadowAtten = 1.0;
    if (uShadowMapSet > 0.0)
    {
        vec4 texShadowView = ShadowViewFromWorld * vec4(worldPosition, 1.0);
        shadowAtten = SampleShadowMapPCFTent(uShadowMap, texShadowView);
    }

    vec3 radiance = MainLightColor * shadowAtten;

    vec3 Lo = vec3(0.0);

    // Disney BRDF diffuse fd
    vec3 Fd = DisneyDiffuse(albedo, NdotL, NdotV, LdotH, perceptualRoughness);

    // Disney BRDF specular
    // Specular D
    float a = perceptualRoughness * perceptualRoughness;
    float Ds = GTR2(NdotH, a);

    // Specular F
    vec3 F0 = mix(vec3(0.04), albedo.rgb, metallic);
    float FH = SchlickFresnel(LdotH);
    vec3 Fs = mix(F0, vec3(1.0), FH);

    // Specular G
    float alphaG = Sqr(perceptualRoughness * 0.5 + 0.5);
    float Gs = SmithG_GGX(NdotL, alphaG);
    Gs *= SmithG_GGX(NdotV, alphaG);

    // Fr
    vec3 BRDF = Fd * (1.0 - metallic) + Ds * Fs * Gs;

    // Li * Fr * cosine
    Lo = radiance * BRDF * NdotL;

    // Environment IBL
    // Environment irradiance
    vec3 irradiance = texture(uIrradianceCubemap, N).rgb;
    Lo += irradiance * Fd;

    // Environment specular
    vec3 R = reflect(-V, N); 
    const float prefilteredCubeMipLevels = 10.0;
    vec3 prefilteredColor = textureLod(uPrefilteredCubemap, R, perceptualRoughness * prefilteredCubeMipLevels).rgb;
    vec2 envBRDF = texture(uBRDFLUT, vec2(max(dot(N, V), 0.0), perceptualRoughness)).rg;
    Lo += prefilteredColor * (F0 * envBRDF.x + envBRDF.y);

    // Occlusion
    Lo *= occlusion;

    // Emissive
    Lo += emission;

    FragColor = vec4(Lo, 1.0);
}