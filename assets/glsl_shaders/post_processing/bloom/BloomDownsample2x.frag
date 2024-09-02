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
    w0 /= (1.0 + Max3(c0.r, c0.g, c0.b));
    w1 /= (1.0 + Max3(c1.r, c1.g, c1.b));
    w2 /= (1.0 + Max3(c2.r, c2.g, c2.b));
    w3 /= (1.0 + Max3(c3.r, c3.g, c3.b));
    float w = 1.0 / (w0 + w1 + w2 + w3);
    w0 *= w;
    w1 *= w;
    w2 *= w;
    w3 *= w;

    vec3 c = c0 * w0 + c1 * w1 + c2 * w2 + c3 * w3;

    // const float k_Softness = 0.5;
    // float threshold = pow(1.0, 2.2);
    // float knee = threshold * k_Softness + 1e-5;
    // vec3 curve = vec3(threshold - knee, knee * 2.0, 0.25 / knee);
    // // Pixel brightness
    // float br = Max3(c.r, c.g, c.b);
    // // Under-threshold part
    // float rq = clamp(br - curve.x, 0.0, curve.y);
    // rq = curve.z * rq * rq;
    // // Combine and apply the brightness response curve
    // c *= max(rq, br - threshold) / max(br, 1e-4);

    OutColor = vec4(c, 1.0);
}