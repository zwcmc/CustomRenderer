#version 410 core
out vec4 FragColor;

in VertexData
{
    vec2 UV0;
    vec3 WorldNormal;
    vec4 WorldTangent;
    vec3 WorldPosition;
    vec4 TexShadowView;
} fs_in;

// Albedo
uniform sampler2D uBaseMap;
uniform float uBaseMapSet;
uniform vec4 uBaseColor;

// Normal
uniform sampler2D uNormalMap;
uniform float uNormalMapSet;

// Emission
uniform sampler2D uEmissiveMap;
uniform float uEmissiveMapSet;
uniform vec3 uEmissiveColor;

// Metallic and roughness
uniform sampler2D uMetallicRoughnessMap;
uniform float uMetallicRoughnessMapSet;
uniform float uMetallicFactor;
uniform float uRoughnessFactor;

// Occlusion
uniform sampler2D uOcclusionMap;
uniform float uOcclusionMapSet;

// Alpha blend
uniform float uAlphaBlendSet;
// Alpha test
uniform float uAlphaTestSet;
uniform float uAlphaCutoff;

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
    vec2 uv = fs_in.UV0;

    vec4 albedo = uBaseMapSet > 0.0 ? SRGBtoLINEAR(texture(uBaseMap, uv)) * uBaseColor : uBaseColor;

    // Alpha test
    if (uAlphaTestSet > 0.0)
    {
        if (albedo.a < uAlphaCutoff)
        {
            discard;
        }
    }

    float metallic = uMetallicFactor;
    float perceptualRoughness = uRoughnessFactor;
    if (uMetallicRoughnessMapSet > 0.0)
    {
        // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
        vec4 metallicRoughness = texture(uMetallicRoughnessMap, uv);
        metallic *= metallicRoughness.b;
        perceptualRoughness *= metallicRoughness.g;
    }

    vec3 N;
    if (uNormalMapSet > 0.0)
    {
        vec3 n = fs_in.WorldNormal;
        vec3 t = fs_in.WorldTangent.xyz;
        vec3 b = cross(n, t) * sign(fs_in.WorldTangent.w);
        mat3 TBN = mat3(t, b, n);
        vec3 tangentNormal = texture(uNormalMap, uv).xyz * 2.0 - 1.0;
        N = normalize(TBN * tangentNormal);
    }
    else
    {
        N = normalize(fs_in.WorldNormal);
    }

    vec3 V = normalize(CameraPosition - fs_in.WorldPosition);
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
        shadowAtten = SampleShadowMapPCFTent(uShadowMap, fs_in.TexShadowView);
    }

    vec3 radiance = MainLightColor * shadowAtten;

    vec3 Lo = vec3(0.0);

    // Disney BRDF diffuse fd
    // vec3 Fd = DisneyDiffuse(albedo, NdotL, NdotV, LdotH, perceptualRoughness);
    vec3 Fd = LambertDiffuse(albedo);

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
    float occlusion = 1.0;
    if (uOcclusionMapSet > 0.0)
    {
        occlusion = texture(uOcclusionMap, uv).r;
    }
    Lo *= occlusion;

    // Emissive
    vec3 emission = uEmissiveMapSet > 0.0 ? SRGBtoLINEAR(texture(uEmissiveMap, uv)).rgb * uEmissiveColor : vec3(0.0, 0.0, 0.0);
    Lo += emission;

    FragColor = vec4(Lo, uAlphaBlendSet > 0.0 ? albedo.a : 1.0); // * CascadeColors[GetCascadeIndex(fs_in.TexShadowView)];
}