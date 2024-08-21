#ifndef SHADOWS_GLSL
#define SHADOWS_GLSL

float sampleShadowmap(sampler2DShadow shadowmap, vec4 shadowCoord)
{
    shadowCoord.xyz /= shadowCoord.w;
    shadowCoord.xyz = shadowCoord.xyz * 0.5 + 0.5;
    return texture(shadowmap, vec3(shadowCoord.xy, shadowCoord.z - 0.001));
}

#endif