#version 410 core
out vec4 OutColor;

in vec2 UV0;

uniform sampler2D uSourceTex;

uniform float uBloomThreshold;

#define CLAMP_MAX 65472.0

#include "common/functions.glsl"

void main()
{
    vec2 uv = UV0;

    vec2 size = textureSize(uSourceTex, 0);
    vec2 texelSize = (vec2(1.0) / size);

    vec4 A = texture(uSourceTex, uv + texelSize * vec2(-1.0, -1.0));
    vec4 B = texture(uSourceTex, uv + texelSize * vec2(0.0, -1.0));
    vec4 C = texture(uSourceTex, uv + texelSize * vec2(1.0, -1.0));
    vec4 D = texture(uSourceTex, uv + texelSize * vec2(-0.5, -0.5));
    vec4 E = texture(uSourceTex, uv + texelSize * vec2(0.5, -0.5));
    vec4 F = texture(uSourceTex, uv + texelSize * vec2(-1.0, 0.0));
    vec4 G = texture(uSourceTex, uv);
    vec4 H = texture(uSourceTex, uv + texelSize * vec2(1.0, 0.0));
    vec4 I = texture(uSourceTex, uv + texelSize * vec2(-0.5, 0.5));
    vec4 J = texture(uSourceTex, uv + texelSize * vec2(0.5, 0.5));
    vec4 K = texture(uSourceTex, uv + texelSize * vec2(-1.0, 1.0));
    vec4 L = texture(uSourceTex, uv + texelSize * vec2(0.0, 1.0));
    vec4 M = texture(uSourceTex, uv + texelSize * vec2(1.0, 1.0));

    vec2 div = (1.0 / 4.0) * vec2(0.5, 0.125);

    vec4 o = (D + E + I + J) * div.x;
    o += (A + B + G + F) * div.y;
    o += (B + C + H + G) * div.y;
    o += (F + G + L + K) * div.y;
    o += (G + H + M + L) * div.y;

    vec3 color = o.xyz;

    color = min(vec3(CLAMP_MAX), color);

    // Half threshold
    float threshold = uBloomThreshold;
    float thresholdKnee = 0.5 * threshold;

    float brightness = Max3(color.r, color.g, color.b);
    float softness = clamp(brightness - threshold + thresholdKnee, 0.0, 2.0 * thresholdKnee);
    softness = (softness * softness) / (4.0 * thresholdKnee + 1e-4);
    float multiplier = max(brightness - threshold, softness) / max(brightness, 1e-4);
    color *= multiplier;

    color = max(color, vec3(0.0));

    OutColor = vec4(color, 1.0);
}