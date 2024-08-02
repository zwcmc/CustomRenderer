#version 410 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexcoord0;
layout (location = 2) in vec3 vNormal;

#include "common/Uniforms.glsl"

uniform mat4 uModelMatrix;
uniform mat3 uModelMatrixInverse;

out vec2 UV0;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    UV0 = vTexcoord0;

    // transpose(uModelMatrixInverse) * vNormal = vNormal * uModelMatrixInverse
    Normal = vNormal * uModelMatrixInverse;

    FragPos = vec3(uModelMatrix * vec4(vPosition, 1.0));

    gl_Position = projectionMatrix * viewMatrix * vec4(FragPos, 1.0);
}