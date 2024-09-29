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
uniform vec4 uEmissiveColor;

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
uniform sampler2D uIBL_DFG;

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

    vec4 baseColor = uBaseMapSet > 0.0 ? SRGBtoLINEAR(texture(uBaseMap, uv)) * uBaseColor : uBaseColor;

    // Alpha test
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
    vec3 L = normalize(MainLightPosition - fs_in.WorldPosition);
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

    vec3 diffuseColor = baseColor.rgb * (1.0 - metallic);
    const float reflectance = 0.04; // Assume 4% reflectance
    vec3 F0 = mix(vec3(reflectance), baseColor.rgb, metallic);

    // Disney BRDF diffuse
    vec3 Fd = diffuseColor * DisneyDiffuse(NdotL, NdotV, LdotH, perceptualRoughness);
    // vec3 Fd = diffuseColor * LambertDiffuse();

    // Disney BRDF specular
    // Specular D
    float a = perceptualRoughness * perceptualRoughness;
    float Ds = GTR2(NdotH, a);

    // Specular F
    float FH = SchlickFresnel(LdotH);
    vec3 Fs = mix(F0, vec3(1.0), FH);

    // Specular G
    float alphaG = Sqr(perceptualRoughness * 0.5 + 0.5);
    float Gs = SmithG_GGX(NdotL, alphaG);
    Gs *= SmithG_GGX(NdotV, alphaG);

    // Fr
    vec3 Fr = Ds * Fs * Gs;

    // BRDF
    vec3 BRDF = Fd + Fr;

    // Lo = Li * BRDF * cosine
    Lo = radiance * BRDF * NdotL;

    // Environment IBL
    // Environment specular
    vec3 R = reflect(-V, N); 
    float lod = PerceptualRoughnessToLod(perceptualRoughness);
    vec3 prefilteredRadiance = textureLod(uPrefilteredCubemap, R, lod).rgb;
    vec2 iblDFG = texture(uIBL_DFG, vec2(NdotV, perceptualRoughness)).rg;
    vec3 E = F0 * iblDFG.x + iblDFG.y;
    vec3 iblFr = prefilteredRadiance * E;

    float diffuseAO = 1.0;//texture(uSSAOTexture, uv).r;
    // Environment irradiance
    vec3 diffuseIrradiance = texture(uIrradianceCubemap, N).rgb;
    vec3 iblFd = Fd * diffuseIrradiance * (1.0 - E) * diffuseAO;

    Lo += iblFr + iblFd;

    // Occlusion
    float occlusion = 1.0;
    if (uOcclusionMapSet > 0.0)
    {
        occlusion = texture(uOcclusionMap, uv).r;
    }
    Lo *= occlusion;

    // Emissive
    vec3 emission = uEmissiveMapSet > 0.0 ? SRGBtoLINEAR(texture(uEmissiveMap, uv)).rgb * uEmissiveColor.rgb : vec3(0.0);
    Lo += emission;

    FragColor = vec4(Lo, uAlphaBlendSet > 0.0 ? baseColor.a : 1.0); // * CascadeColors[GetCascadeIndex(fs_in.TexShadowView)];
}