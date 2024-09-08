#ifndef UNIFORMS_GLSL
#define UNIFORMS_GLSL

layout (std140) uniform GlobalUniforms
{
    mat4 ViewFromWorld; // view
    mat4 ClipFromView;  // projection
    vec3 MainLightDirection;
    vec3 MainLightColor;
    vec3 CameraPosition;
    mat4 ShadowClipFromView[4];
    mat4 ShadowViewFromWorld;
    vec4 CascadeScalesAndOffsets[4];
    vec4 CascadeParams; // { x: cascades count, y: min border padding (1.0f/bufferSize), z: max border padding (1.0 - 1.0f/bufferSize), w: unused }
    vec4 ShadowMapTexelSize; // { x: 1.0 / width, y: 1.0 / height, z: width, w: height }
};

uniform float uFXAASet;
uniform float uToneMappingSet;

#endif