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

// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 ACES(vec3 col)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((col*(a*col+b))/(col*(c*col+d)+e), 0.0, 1.0);
}

// http://filmicworlds.com/blog/filmic-tonemapping-operators/

#endif