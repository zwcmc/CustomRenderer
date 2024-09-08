#version 410 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec4 vTangents;
layout (location = 2) in vec2 vTexcoord0;

#include "common/uniforms.glsl"
#include "common/functions.glsl"

uniform mat4 uModelToWorld;
uniform mat3 uModelNormalToWorld;

out GBufferVertexData
{
    vec2 UV0;
    vec3 WorldNormal;
    vec4 WorldTangent; // xyz: tangent, w: the sign of the bitangent
    vec3 WorldPosition;
} gbuffer_vs_out;

void main()
{
    gbuffer_vs_out.UV0 = vTexcoord0;

    // Extract the normal and tangent from the input quaternion
    vec3 normal;
    vec3 tangent;
    ExtractNormalAndTangent(vTangents, normal, tangent);

    gbuffer_vs_out.WorldNormal = uModelNormalToWorld * normal;
    gbuffer_vs_out.WorldTangent.xyz = uModelNormalToWorld * tangent;
    gbuffer_vs_out.WorldTangent.w = vTangents.w;

    gbuffer_vs_out.WorldPosition = vec3(uModelToWorld * vec4(vPosition, 1.0));

    gl_Position = ClipFromView * ViewFromWorld * vec4(gbuffer_vs_out.WorldPosition, 1.0);
}