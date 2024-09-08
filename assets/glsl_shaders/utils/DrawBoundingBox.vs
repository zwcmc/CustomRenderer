#version 410 core

layout (location = 0) in vec3 vPosition;

#include "common/uniforms.glsl"

uniform mat4 uModelToWorld;

void main()
{
    gl_Position = ClipFromView * ViewFromWorld * uModelToWorld * vec4(vPosition, 1.0);
}