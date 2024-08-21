#version 410 core
out vec4 FragColor;

in VertexData
{
    vec2 UV0;
    vec3 Normal;
    vec3 PositionWS;
} fs_in;

uniform sampler2D uBaseMap;
uniform float uBaseMapSet;
uniform vec4 uBaseColor;

uniform sampler2D uNormalMap;
uniform float uNormalMapSet;

uniform sampler2D uEmissiveMap;
uniform float uEmissiveMapSet;
uniform vec3 uEmissiveColor;

uniform sampler2D uMetallicRoughnessMap;
uniform float uMetallicRoughnessMapSet;
uniform float uMetallicFactor;
uniform float uRoughnessFactor;

uniform sampler2D uOcclusionMap;
uniform float uOcclusionMapSet;

uniform float uAlphaTestSet;
uniform float uAlphaCutoff;

uniform samplerCube uIrradianceCubemap;
uniform samplerCube uPrefilteredCubemap;
uniform sampler2D uBRDFLUT;

uniform sampler2DShadow uShadowmap;

#include "pbr/brdfs.glsl"
#include "common/uniforms.glsl"
#include "common/functions.glsl"
#include "shader_library/shadows.glsl"

void main()
{
    vec4 baseColor = uBaseMapSet > 0.0 ? SRGBtoLINEAR(texture(uBaseMap, fs_in.UV0)) * uBaseColor : uBaseColor;

    if (uAlphaTestSet > 0.0)
    {
        if (baseColor.a < uAlphaCutoff)
        {
            discard;
        }
    }

    float metallic = uMetallicFactor;
    float perceptualRoughness = uRoughnessFactor;
    if (uMetallicRoughnessMapSet > 0.0)
    {
        // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
        vec4 metallicRoughness = texture(uMetallicRoughnessMap, fs_in.UV0);
        metallic *= metallicRoughness.b;
        perceptualRoughness *= metallicRoughness.g;
    }

    vec3 N = uNormalMapSet > 0.0 ? getNormalWS(uNormalMap, fs_in.PositionWS, fs_in.Normal, fs_in.UV0) : normalize(fs_in.Normal);
    vec3 V = normalize(cameraPos - fs_in.PositionWS);
    vec3 L = normalize(lightPosition0);
    vec3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float LdotH = max(dot(L, H), 0.0);


    // Main light shadow
    vec4 shadowCoord = getShadowCoord(fs_in.PositionWS);
    float shadowAtten = sampleShadowmapTent5x5(uShadowmap, shadowCoord);

    vec3 radiance = lightColor0 * shadowAtten;

    vec3 Lo = vec3(0.0);

    // Disney BRDF diffuse fd
    // vec3 Fd = DisneyDiffuse(baseColor.rgb, NdotL, NdotV, LdotH, perceptualRoughness);
    vec3 Fd = LambertDiffuse(baseColor.rgb);

    // Disney BRDF specular
    // Specular D
    float a = perceptualRoughness * perceptualRoughness;
    float Ds = GTR2(NdotH, a);

    // Specular F
    vec3 F0 = mix(vec3(0.04), baseColor.rgb, metallic);
    float FH = SchlickFresnel(LdotH);
    vec3 Fs = mix(F0, vec3(1.0), FH);

    // Specular G
    float alphaG = sqr(perceptualRoughness * 0.5 + 0.5);
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
    if (uOcclusionMapSet > 0.0)
    {
        float ao = texture(uOcclusionMap, fs_in.UV0).r;
        Lo *= ao;
    }

    // Emissive
    vec3 emission = uEmissiveMapSet > 0.0 ? SRGBtoLINEAR(texture(uEmissiveMap, fs_in.UV0)).rgb * uEmissiveColor : vec3(0.0, 0.0, 0.0);
    Lo += emission;

    FragColor = vec4(Lo, baseColor.a);
}