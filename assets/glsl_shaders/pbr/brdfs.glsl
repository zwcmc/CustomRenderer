#ifndef BRDFS_GLSL
#define BRDFS_GLSL

#include "common/constants.glsl"

float SchlickFresnel(float u)
{
    // pow((1.0 - u), 5.0)
    float m = clamp(1.0 - u, 0.0, 1.0);
    float m2 = m * m;
    return m2 * m2 * m; // pow(m, 5.0)
}

float LambertDiffuse()
{
    return INV_PI;
}

float DisneyDiffuse(float NdotL, float NdotV, float LdotH, float perceptualRoughness)
{
    float Fd90 = 0.5 + 2.0 * perceptualRoughness * LdotH * LdotH;

    float FL = SchlickFresnel(NdotL);
    float FV = SchlickFresnel(NdotV);

    return INV_PI * mix(1.0, Fd90, FL) * mix(1.0, Fd90, FV); // (1.0 / π ) * (1.0 + (Fd90 - 1.0) * FL)(1.0 + (Fd90 - 1.0) * FV)
}

// GTR, Generalized-Trowbridge-Reitz
// \gamma = 1, Berry Distribution
float GTR1(float NdotH, float a)
{
    if (a >= 1.0) return 1.0 / M_PI;

    float a2 = a * a;
    float t = 1.0 + (a2 - 1.0) * NdotH * NdotH;
    return (a2 - 1.0) / (M_PI * log(a2) * t);
}

// \gamma = 2, GGX(Trowbridge-Reitz) Distribution
float GTR2(float NdotH, float a)
{
    float a2 = a * a;
    float t = 1.0 + (a2 - 1.0) * NdotH * NdotH;
    return a2 / (M_PI * t * t);
}

// Smith-G-GGX
// alphaG = (0.5 * roughness + 0.5)^2
float SmithG_GGX(float NdotV, float alphaG)
{
    float a2 = alphaG * alphaG;
    float b = NdotV * NdotV;
    return 1.0 / (NdotV + sqrt(a2 + b - a2 * b));
}

// Smith-G-GGX Epic
// alphaG = (0.5 * roughness + 0.5)^2
float SmithG_GGX_Epic(float NdotV, float alphaG)
{
    float k = 0.5 * alphaG;
    return NdotV / (NdotV * (1.0 - k) + k);
}

// Smith-G-GGX Epic IBL
// a = roughness^2
float SmithG_GGX_EpicIBL(float NdotV, float a)
{
    float k = 0.5 * a;
    return NdotV / (NdotV * (1.0 - k) + k);
}

#endif