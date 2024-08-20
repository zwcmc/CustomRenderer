#ifndef UNIFORMS_GLSL
#define UNIFORMS_GLSL

layout (std140) uniform GlobalUniforms
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec3 lightDirection0;
    vec3 lightColor0;
    vec3 cameraPos;
    mat4 worldToLight;
};

#endif