#version 410 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec4 vTangents;
layout (location = 2) in vec2 vTexcoord0;

#include "common/uniforms.glsl"
#include "common/functions.glsl"

uniform mat4 uModelMat;
uniform mat3 uInverseModelMat;

out VertexData
{
    vec2 UV0;
    vec3 WorldNormal;
    vec4 WorldTangent; // xyz: tangent, w: sign
    vec3 WorldPosition;
    vec4 TexShadowView; // Position in light shadow view space
} vs_out;

void main()
{
    vs_out.UV0 = vTexcoord0;

    // Extract the normal and tangent from the input quaternion
    vec3 normal;
    vec3 tangent;
    ExtractNormalAndTangent(vTangents, normal, tangent);

    // transpose(uInverseModelMat) * v = v * uInverseModelMat
    vs_out.WorldNormal = normal * uInverseModelMat;
    vs_out.WorldTangent.xyz = tangent * uInverseModelMat;
    vs_out.WorldTangent.w = vTangents.w;

    vs_out.WorldPosition = vec3(uModelMat * vec4(vPosition, 1.0));

    vs_out.TexShadowView = ShadowView * vec4(vs_out.WorldPosition, 1.0);

    gl_Position = ProjectionMatrix * ViewMatrix * vec4(vs_out.WorldPosition, 1.0);
}