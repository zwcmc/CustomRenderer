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

uniform sampler2D uBaseMap;
uniform float uBaseMapSet;
uniform vec4 uBaseColor;

uniform sampler2D uNormalMap;
uniform float uNormalMapSet;

uniform sampler2DShadow uShadowMap;
uniform float uShadowMapSet;

#include "common/uniforms.glsl"
#include "common/functions.glsl"
#include "shadows/shadows.glsl"

void CalculateRightAndUpTexelDepthDeltas(in vec3 texShadowView, in mat3 shadowProjection, out float upTextDepthWeight, out float rightTextDepthWeight)
{
    vec3 vShadowTexDDX = dFdx(texShadowView);
    vec3 vShadowTexDDY = dFdy(texShadowView);

    vShadowTexDDX = shadowProjection * vShadowTexDDX;
    vShadowTexDDY = shadowProjection * vShadowTexDDY;

    mat2 matScreenToShadow = mat2(vShadowTexDDX.xy, vShadowTexDDY.xy);
    float fDeterminant = determinant(matScreenToShadow);

    float fInvDeterminant = 1.0 / fDeterminant;

    mat2 matShadowToScreen = mat2(
        matScreenToShadow[1][1] * fInvDeterminant, matScreenToShadow[0][1] * -fInvDeterminant,
        matScreenToShadow[1][0] * -fInvDeterminant, matScreenToShadow[0][0] * fInvDeterminant
    );

    vec2 vRightShadowTexelLocation = vec2(ShadowMapTexelSize.x, 0.0);
    vec2 vUpShadowTexelLocation = vec2(0.0, ShadowMapTexelSize.y);

    vec2 vRightTexelDepthRatio = matShadowToScreen * vRightShadowTexelLocation;
    vec2 vUpTexelDepthRatio = matShadowToScreen * vUpShadowTexelLocation;

    upTextDepthWeight = vUpTexelDepthRatio.x * vShadowTexDDX.z + vUpTexelDepthRatio.y * vShadowTexDDY.z;
    rightTextDepthWeight = vRightTexelDepthRatio.x * vShadowTexDDX.z + vRightTexelDepthRatio.y * vShadowTexDDY.z;
}

void main()
{
    vec2 uv = fs_in.UV0;

    vec4 albedo = uBaseMapSet > 0.0 ? SRGBtoLINEAR(texture(uBaseMap, uv)) * uBaseColor : uBaseColor;

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

    float shadowAtten = 1.0;
    if (uShadowMapSet > 0.0)
    {
        shadowAtten = SampleShadowMapPCFTent(uShadowMap, fs_in.TexShadowView);
    }

    vec3 radiance = MainLightColor * shadowAtten;

    vec3 ambient = albedo.rgb * 0.04;
    vec3 diffuse = albedo.rgb * radiance * NdotL;
    float spec = pow(max(dot(N, H), 0.0), 32.0);
    vec3 specular = vec3(1.0) * radiance * spec;

    vec3 color = ambient + diffuse + specular;

    FragColor = vec4(color, 1.0); // * CascadeColors[GetCascadeIndex(fs_in.TexShadowView)];
}