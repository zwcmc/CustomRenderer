#version 410 core
out vec4 OutColor;

in VertexData
{
    vec2 UV0;
    vec3 Normal;
    vec3 PositionWS;
    // float Depth;
    vec4 vTexShadow;
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
#include "shader_library/shadows.glsl"

const vec4 CascadeColors[4] = vec4[4]
(
    vec4(1.0, 0.0, 0.0, 1.0),
    vec4(0.0, 1.0, 0.0, 1.0),
    vec4(0.0, 0.0, 1.0, 1.0),
    vec4(1.0, 0.0, 1.0, 1.0)
);

void main()
{
    vec4 baseColor = uBaseMapSet > 0.0 ? SRGBtoLINEAR(texture(uBaseMap, fs_in.UV0)) * uBaseColor : uBaseColor;

    vec3 N = uNormalMapSet > 0.0 ? GetNormalWS(uNormalMap, fs_in.PositionWS, fs_in.Normal, fs_in.UV0) : normalize(fs_in.Normal);
    vec3 V = normalize(CameraPosition - fs_in.PositionWS);
    vec3 L = normalize(MainLightDirection);
    vec3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0);

    float shadowAtten = 1.0;
    int iCurrentCascadeIndex = 0;
    if (uShadowMapSet > 0.0)
    {
        int CASCADE_COUNT_FLAG = int(CascadeParams.x);
        vec4 shadowCoord = vec4(0.0);
        int iCascadeFound = 0;
        for (int iCascadeIndex = 0; iCascadeIndex < CASCADE_COUNT_FLAG && iCascadeFound == 0; ++iCascadeIndex)
        {
            shadowCoord = ShadowProjections[iCascadeIndex] * fs_in.vTexShadow;
            // Perspective division
            shadowCoord.xyzw /= shadowCoord.w;
            if (min(shadowCoord.x, shadowCoord.y) > CascadeParams.y && max(shadowCoord.x, shadowCoord.y) < CascadeParams.z)
            {
                iCurrentCascadeIndex = iCascadeIndex;
                iCascadeFound = 1;
            }
        }
        shadowCoord.xy = shadowCoord.xy * CascadeScalesAndOffsets[iCurrentCascadeIndex].xy + CascadeScalesAndOffsets[iCurrentCascadeIndex].zw;
        shadowAtten = SampleShadowMapTent5x5(uShadowMap, shadowCoord);
    }
    vec3 radiance = MainLightColor * shadowAtten;

    vec3 ambient = baseColor.rgb * 0.04;
    vec3 diffuse = baseColor.rgb * radiance * NdotL;
    float spec = pow(max(dot(N, H), 0.0), 32.0);
    vec3 specular = vec3(1.0) * radiance * spec;

    vec3 color = ambient + diffuse + specular;

    OutColor = vec4(color, 1.0) * CascadeColors[iCurrentCascadeIndex];
}