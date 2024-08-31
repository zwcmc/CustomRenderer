#version 410 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexcoord0;
layout (location = 2) in vec3 vNormal;

#include "common/uniforms.glsl"

uniform mat4 uModelMatrix;
uniform mat3 uModelMatrixInverse;

out VertexData
{
    vec2 UV0; // Texcoord 0
    vec3 Normal; // Normal in world space
    vec3 PositionWS; // Position in world spcae
    vec4 TexShadowView; // Position in light shadow view space
} vs_out;

void main()
{
    vs_out.UV0 = vTexcoord0;

    // transpose(uModelMatrixInverse) * vNormal = vNormal * uModelMatrixInverse
    vs_out.Normal = vNormal * uModelMatrixInverse;

    vs_out.PositionWS = vec3(uModelMatrix * vec4(vPosition, 1.0));

    vs_out.TexShadowView = ShadowView * vec4(vs_out.PositionWS, 1.0);

    gl_Position = ProjectionMatrix * ViewMatrix * vec4(vs_out.PositionWS, 1.0);
}