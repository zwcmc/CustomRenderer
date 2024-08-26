#version 410 core
out vec4 OutColor;

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

uniform sampler2DShadow uShadowmap;

#include "common/uniforms.glsl"
#include "common/functions.glsl"
#include "shader_library/shadows.glsl"

void main()
{
    vec4 baseColor = uBaseMapSet > 0.0 ? SRGBtoLINEAR(texture(uBaseMap, fs_in.UV0)) * uBaseColor : uBaseColor;

    vec3 N = uNormalMapSet > 0.0 ? GetNormalWS(uNormalMap, fs_in.PositionWS, fs_in.Normal, fs_in.UV0) : normalize(fs_in.Normal);
    vec3 V = normalize(CameraPosition - fs_in.PositionWS);
    vec3 L = normalize(MainLightDirection);
    vec3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0);

    vec4 shadowCoord = GetShadowCoord(fs_in.PositionWS);

    OutColor = vec4(vec3(shadowCoord.z), 1.0);

    // float shadowAtten = SampleShadowmap(uShadowmap, shadowCoord);

    // vec3 radiance = MainLightColor * shadowAtten;

    // vec3 ambient = baseColor.rgb * 0.04;

    // vec3 diffuse = baseColor.rgb * radiance * NdotL;

    // float spec = pow(max(dot(N, H), 0.0), 32.0);
    // vec3 specular = vec3(1.0) * radiance * spec;

    // OutColor = vec4(ambient + diffuse + specular, 1.0);
}