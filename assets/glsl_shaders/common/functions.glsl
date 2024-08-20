#ifndef FUNCTIONS_GLSL
#define FUNCTIONS_GLSL

#include "common/constants.glsl"

float sqr(float x)
{
    return x * x;
}

float pow5(float x)
{
    return sqr(x) * sqr(x) * x;
}

// The specific texture is not indicated when loading glTF, so the sRGB conversion for the texture can only be specified in the shader
vec4 SRGBtoLINEAR(vec4 srgbIn)
{
    vec3 linOut = pow(srgbIn.xyz, vec3(GAMMA));
    return vec4(linOut, srgbIn.w);
}

vec4 GammaCorrection(vec4 color)
{
    return vec4(pow(color.rgb, vec3(INV_GAMMA)), color.a);
}

// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// efficient VanDerCorpus calculation.
vec2 Hammersley2d(uint i, uint N)
{
    uint bits = (i << 16u) | (i >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    float rdi =  float(bits) * 2.3283064365386963e-10; // / 0x100000000
    return vec2(float(i) / float(N), rdi);
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = sqr(roughness);

    float phi = M_TAU * Xi.x;
    // float theta = atan(a * sqrt(Xi.y) / sqrt(1.0 - Xi.y));
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 T = normalize(cross(up, N));
    vec3 B = cross(N, T);

    vec3 sampleVec = T * H.x + B * H.y + N * H.z;
    return normalize(sampleVec);
}

#endif