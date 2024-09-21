#version 410 core
out vec4 FragColor;

in vec2 UV0;

uniform sampler2D uSourceTex;
uniform vec4 uOffset;

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

vec4 Blur(const highp vec2 uv, const highp vec2 delta)
{
    vec4 p0 = texture(uSourceTex, uv);
    vec4 p1a = texture(uSourceTex, uv - delta * 1.3846153846);
    vec4 p1b = texture(uSourceTex, uv + delta * 1.3846153846);
    vec4 p2a = texture(uSourceTex, uv - delta * 3.2307692308);
    vec4 p2b = texture(uSourceTex, uv + delta * 3.2307692308);

    vec3 n0 = GetPackedNormal(p0);

    float w0  =                                           float(0.2270270270);
    float w1a = CompareNormal(n0, GetPackedNormal(p1a)) * float(0.3162162162);
    float w1b = CompareNormal(n0, GetPackedNormal(p1b)) * float(0.3162162162);
    float w2a = CompareNormal(n0, GetPackedNormal(p2a)) * float(0.0702702703);
    float w2b = CompareNormal(n0, GetPackedNormal(p2b)) * float(0.0702702703);

    float s = float(0.0);
    s += GetPackedAO(p0)  * w0;
    s += GetPackedAO(p1a) * w1a;
    s += GetPackedAO(p1b) * w1b;
    s += GetPackedAO(p2a) * w2a;
    s += GetPackedAO(p2b) * w2b;
    s *= Rcp(w0 + w1a + w1b + w2a + w2b);

    return vec4(s, n0);
}

void main()
{
    vec2 uv = UV0;
    FragColor = Blur(uv, uOffset.xy);
}