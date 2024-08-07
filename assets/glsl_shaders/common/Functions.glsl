#ifndef FUNCTIONS_GLSL
#define FUNCTIONS_GLSL

#include "common/Defines.glsl"

// The specific texture is not indicated when loading glTF, so the sRGB conversion for the texture can only be specified in the shader
vec4 SRGBtoLINEAR(vec4 srgbIn)
{
    vec3 linOut = pow(srgbIn.xyz, vec3(GAMMA));
    return vec4(linOut, srgbIn.w);
}

vec3 GammaCorrection(vec3 color)
{
    return pow(color, vec3(INV_GAMMA));
}

#endif