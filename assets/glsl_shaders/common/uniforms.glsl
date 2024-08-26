#ifndef UNIFORMS_GLSL
#define UNIFORMS_GLSL

layout (std140) uniform GlobalUniforms
{
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    vec3 MainLightDirection;
    vec3 MainLightColor;
    vec3 CameraPosition;
    mat4 WorldToShadows[4];
    float CascadePartitionsFrustum[4];
};

#endif