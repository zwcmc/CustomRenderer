#version 410 core

layout (location = 0) in vec3 vPosition;

#include "common/uniforms.glsl"

out vec3 UVW;

void main()
{
    UVW = vPosition;

    vec4 clipPos = ProjectionMatrix * mat4(mat3(ViewMatrix)) * vec4(vPosition, 1.0);

    gl_Position = clipPos.xyww;
}