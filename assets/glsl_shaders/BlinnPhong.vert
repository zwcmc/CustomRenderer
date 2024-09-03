#version 410 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec4 vTangent;
layout (location = 3) in vec2 vTexcoord0;

#include "common/uniforms.glsl"

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

void ExtractNormal(in vec4 q, out vec3 n)
{
    n = vec3( 0.0,  0.0,  1.0) +
        vec3( 2.0, -2.0, -2.0) * q.x * q.zwx +
        vec3( 2.0,  2.0, -2.0) * q.y * q.wzy;
}

void ExtractNormalAndTangent(in vec4 q, out vec3 n, out vec3 t)
{
    ExtractNormal(q, n);
    t = vec3( 1.0,  0.0,  0.0) +
        vec3(-2.0,  2.0, -2.0) * q.y * q.yxw +
        vec3(-2.0,  2.0,  2.0) * q.z * q.zwx;
}

void main()
{
    vs_out.UV0 = vTexcoord0;

    // // transpose(uInverseModelMat) * vNormal = vNormal * uInverseModelMat
    // vs_out.WorldNormal = vNormal * uInverseModelMat;

    vec3 normal;
    vec3 tangent;
    ExtractNormalAndTangent(vTangent, normal, tangent);
    vs_out.WorldNormal = normal * uInverseModelMat;
    vs_out.WorldTangent.xyz = tangent * uInverseModelMat;
    vs_out.WorldTangent.w = vTangent.w;

    vs_out.WorldPosition = vec3(uModelMat * vec4(vPosition, 1.0));

    vs_out.TexShadowView = ShadowView * vec4(vs_out.WorldPosition, 1.0);

    gl_Position = ProjectionMatrix * ViewMatrix * vec4(vs_out.WorldPosition, 1.0);
}