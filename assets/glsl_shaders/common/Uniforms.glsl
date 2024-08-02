#ifndef UNIFORMS
#define UNIFORMS

layout (std140) uniform GlobalUniforms
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

#endif