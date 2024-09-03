#version 410 core
out vec4 OutColor;

in VertexData
{
    vec2 UV0;
    vec3 Normal;
    vec3 PositionWS;
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
    vec4 baseColor = uBaseMapSet > 0.0 ? SRGBtoLINEAR(texture(uBaseMap, fs_in.UV0)) * uBaseColor : uBaseColor;

    vec3 N = uNormalMapSet > 0.0 ? GetNormalWS(uNormalMap, fs_in.PositionWS, fs_in.Normal, fs_in.UV0) : normalize(fs_in.Normal);
    vec3 V = normalize(CameraPosition - fs_in.PositionWS);
    vec3 L = normalize(MainLightDirection);
    vec3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0);

    float shadowAtten = 1.0;
    if (uShadowMapSet > 0.0)
    {
        shadowAtten = SampleShadowMapPCFTent(uShadowMap, fs_in.TexShadowView);
    }

    vec3 radiance = MainLightColor * shadowAtten;

    vec3 ambient = baseColor.rgb * 0.04;
    vec3 diffuse = baseColor.rgb * radiance * NdotL;
    float spec = pow(max(dot(N, H), 0.0), 32.0);
    vec3 specular = vec3(1.0) * radiance * spec;

    vec3 color = ambient + diffuse + specular;

    OutColor = vec4(color, 1.0); // * CascadeColors[GetCascadeIndex(fs_in.TexShadowView)];
}