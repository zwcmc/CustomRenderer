#version 410 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexcoord0;

#include "common/Uniforms.glsl"

uniform mat4 uModelMatrix;

out vec2 UV0;

void main()
{
    UV0 = vTexcoord0;

    gl_Position = projectionMatrix * viewMatrix * uModelMatrix * vec4(vPosition, 1.0);
}