#ifndef TONEMAPPING_GLSL
#define TONEMAPPING_GLSL

#include "common/constants.glsl"
#include "common/functions.glsl"

const float exposureBias = 1.0;

vec3 ReinhardToneMapping(vec3 color)
{
    color *= exposureBias;
    return color / (1.0 + color);
}

vec3 Uncharted2Tonemap(vec3 color)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    return ((color*(A*color+C*B)+D*E)/(color*(A*color+B)+D*F))-E/F;
}

// From http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 FilmicTonemapping(vec3 color)
{
    const float W = 11.2;

    vec3 outcol = Uncharted2Tonemap(color.rgb * exposureBias);
    vec3 whiteScale = 1.0 / Uncharted2Tonemap(vec3(W));
    outcol = outcol * whiteScale;
    return outcol;
}

// From https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 ACESFilm(vec3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), vec3(0.0), vec3(1.0));
}

// From https://github.com/KhronosGroup/ToneMapping/blob/main/PBR_Neutral/pbrNeutral.glsl
// Input color is non-negative and resides in the Linear Rec. 709 color space.
// Output color is also Linear Rec. 709, but in the [0, 1] range.
vec3 NeutralTonemapping(vec3 color)
{
    color = max(vec3(0.0), color);

    const float startCompression = 0.8 - 0.04;
    const float desaturation = 0.15;

    float x = min(color.r, min(color.g, color.b));
    float offset = x < 0.08 ? x - 6.25 * x * x : 0.04;
    color -= offset;

    float peak = Max3(color);
    if (peak < startCompression) return color;

    const float d = 1.0 - startCompression;
    float newPeak = 1.0 - d * d / (peak + d - startCompression);
    color *= newPeak / peak;

    float g = 1.0 - 1.0 / (desaturation * (peak - newPeak) + 1.0);
    return mix(color, newPeak * vec3(1.0), g);
}

#endif