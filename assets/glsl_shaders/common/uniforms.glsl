#ifndef UNIFORMS_GLSL
#define UNIFORMS_GLSL

layout (std140) uniform GlobalUniforms
{
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    vec3 MainLightDirection;
    vec3 MainLightColor;
    vec3 CameraPosition;
    mat4 ShadowProjections[4];
    mat4 ShadowView;
    vec4 CascadeScalesAndOffsets[4];
};

#endif