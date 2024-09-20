#version 410 core
out vec4 FragColor;

in vec2 UV0;

uniform sampler2D uSourceTex;
uniform vec4 uSourceTexSize; // { x: 1.0/width, y: 1.0/height, z: width, w: height }

#include "common/functions.glsl"

// The constant below controls the geometry-awareness of the bilateral
// filter. The higher value, the more sensitive it is.
const float kGeometryCoeff = float(0.8);

vec3 GetPackedNormal(vec4 p)
{
    return p.gba * 2.0 - 1.0;
}

float CompareNormal(vec3 d1, vec3 d2)
{
    return smoothstep(kGeometryCoeff, 1.0, dot(d1, d2));
}

float GetPackedAO(vec4 p)
{
    return p.r;
}

float BlurSmall(const highp vec2 uv, const vec2 delta)
{
    vec4 p0 = texture(uSourceTex, uv                           );
    vec4 p1 = texture(uSourceTex, uv + vec2(-delta.x, -delta.y));
    vec4 p2 = texture(uSourceTex, uv + vec2( delta.x, -delta.y));
    vec4 p3 = texture(uSourceTex, uv + vec2(-delta.x,  delta.y));
    vec4 p4 = texture(uSourceTex, uv + vec2( delta.x,  delta.y));

    vec3 n0 = GetPackedNormal(p0);

    float w0 = 1.0;
    float w1 = CompareNormal(n0, GetPackedNormal(p1));
    float w2 = CompareNormal(n0, GetPackedNormal(p2));
    float w3 = CompareNormal(n0, GetPackedNormal(p3));
    float w4 = CompareNormal(n0, GetPackedNormal(p4));

    float s = 0.0;
    s += GetPackedAO(p0) * w0;
    s += GetPackedAO(p1) * w1;
    s += GetPackedAO(p2) * w2;
    s += GetPackedAO(p3) * w3;
    s += GetPackedAO(p4) * w4;

    return s *= Rcp(w0 + w1 + w2 + w3 + w4);
}

void main()
{
    vec2 uv = UV0;
    vec2 texelSize = uSourceTexSize.xy;

    FragColor = vec4(vec3(1.0) - BlurSmall(uv, texelSize), 1.0);
}