#version 410 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexcoord0;
layout (location = 2) in vec3 inNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 UV0;
out vec3 Normal;

void main()
{
    UV0 = inTexcoord0;
    Normal = inNormal;
    gl_Position = projection * view * model * vec4(inPos, 1.0);
}