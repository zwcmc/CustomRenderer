#version 410 core
out vec4 OutColor;

in vec2 UV0;

uniform sampler2D uSourceTex;

#include "common/functions.glsl"

void main()
{
    vec2 size = textureSize(uSourceTex, 0);
    vec2 texelSize = (vec2(1.0) / size);

    // Castaño, 2013, "Shadow Mapping Summary Part 1"
    // 3x3 gaussian filter with 4 linear samples
    // From https://github.com/google/filament
    vec2 offset = vec2(0.5);
    vec2 uv = (UV0.xy * size) + offset;
    vec2 base = (floor(uv) - offset) * texelSize;
    vec2 st = fract(uv);
    vec2 uw = vec2(3.0 - 2.0 * st.x, 1.0 + 2.0 * st.x);
    vec2 vw = vec2(3.0 - 2.0 * st.y, 1.0 + 2.0 * st.y);
    vec2 u = vec2((2.0 - st.x) / uw.x - 1.0, st.x / uw.y + 1.0) * texelSize.x;
    vec2 v = vec2((2.0 - st.y) / vw.x - 1.0, st.y / vw.y + 1.0) * texelSize.y;
    vec3 c0 = textureLod(uSourceTex, base + vec2(u.x, v.x), 0.0).rgb;
    vec3 c1 = textureLod(uSourceTex, base + vec2(u.y, v.x), 0.0).rgb;
    vec3 c2 = textureLod(uSourceTex, base + vec2(u.x, v.y), 0.0).rgb;
    vec3 c3 = textureLod(uSourceTex, base + vec2(u.y, v.y), 0.0).rgb;

    float w0 = uw.x * vw.x * (1.0 / 16.0);
    float w1 = uw.y * vw.x * (1.0 / 16.0);
    float w2 = uw.x * vw.y * (1.0 / 16.0);
    float w3 = uw.y * vw.y * (1.0 / 16.0);

    // Solve fireflies
    w0 /= (1.0 + Max3(c0));
    w1 /= (1.0 + Max3(c1));
    w2 /= (1.0 + Max3(c2));
    w3 /= (1.0 + Max3(c3));
    float w = 1.0 / (w0 + w1 + w2 + w3);
    w0 *= w;
    w1 *= w;
    w2 *= w;
    w3 *= w;

    vec3 c = c0 * w0 + c1 * w1 + c2 * w2 + c3 * w3;

    // Thresholding from HDRP
    // const float k_Softness = 0.5;
    // float threshold = 1.0;
    // float knee = threshold * k_Softness + 1e-5;
    // vec3 curve = vec3(threshold - knee, knee * 2.0, 0.25 / knee);
    // // Pixel brightness
    // float br = Max3(c);
    // // Under-threshold part
    // float rq = clamp(br - curve.x, 0.0, curve.y);
    // rq = curve.z * rq * rq;
    // // Combine and apply the brightness response curve
    // c *= max(rq, br - threshold) / max(br, 1e-4);

    // Thresholding from filament
    // threshold everything below 1.0
    c = max(vec3(0.0), c - 1.0);
    // crush everything above 1
    highp float f = Max3(c);
    c *= 1.0 / (1.0 + f * 0.01);

    OutColor = vec4(c, 1.0);
}