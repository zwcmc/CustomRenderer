#version 410 core

layout (location = 0) in vec3 vPosition;

#include "common/Uniforms.glsl"

out vec3 WorldPos;

void main()
{
    WorldPos = vPosition;

    vec4 clipPos = projectionMatrix * mat4(mat3(viewMatrix)) * vec4(vPosition, 1.0);

    gl_Position = clipPos.xyww;
}