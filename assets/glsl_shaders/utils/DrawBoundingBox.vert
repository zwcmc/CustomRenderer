#version 410 core

layout (location = 0) in vec3 vPosition;

#include "common/uniforms.glsl"

uniform mat4 uModelMatrix;

void main()
{
    gl_Position = ProjectionMatrix * ViewMatrix * uModelMatrix * vec4(vPosition, 1.0);
}