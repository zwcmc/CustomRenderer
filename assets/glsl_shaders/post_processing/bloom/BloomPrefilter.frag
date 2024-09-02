#version 410 core
out vec4 OutColor;

in vec2 UV0;

uniform sampler2D uSourceTex;

uniform float uBloomThreshold;

#include "common/functions.glsl"

// From Unity HDRP: Quadratic color thresholding
// curve = (threshold - knee, knee * 2, 0.25 / knee)
vec3 QuadraticThreshold(vec3 color, float threshold, vec3 curve)
{
    // Pixel brightness
    float br = Max3(color.r, color.g, color.b);

    // Under-threshold part
    float rq = clamp(br - curve.x, 0.0, curve.y);
    rq = curve.z * rq * rq;

    // Combine and apply the brightness response curve
    color *= max(rq, br - threshold) / max(br, 1e-4);

    return color;
}

vec3 BilinearSample(sampler2D tex, vec2 uv, out float weight)
{
    vec4 c = textureLod(tex, uv, 0.0);

    const float k_Softness = 0.5;
    float lthresh = pow(uBloomThreshold, 2.2);
    float knee = lthresh * k_Softness + 1e-5;
    vec3 curve = vec3(lthresh - knee, knee * 2.0, 0.25 / knee);
    c.rgb = QuadraticThreshold(c.rgb, lthresh, curve);

    weight = 1.0 / (Luminance(c) + 1.0);

    return c.rgb;
}

vec3 PartialAverage(vec3 c0, vec3 c1, vec3 c2, vec3 c3, float w0, float w1, float w2, float w3)
{
    return (c0*w0 + c1*w1 + c2*w2 + c3*w3) / (w0 + w1 + w2 + w3);
}

void main()
{
    vec2 uv = UV0;

    vec2 size = textureSize(uSourceTex, 0);
    vec2 texelSize = (vec2(1.0) / size);

    float wA, wB, wC, wD, wE, wF, wG, wH, wI, wJ, wK, wL, wM;
    vec3 A = BilinearSample(uSourceTex, uv + texelSize * vec2(-1.0, -1.0), wA);
    vec3 B = BilinearSample(uSourceTex, uv + texelSize * vec2( 0.0, -1.0), wB);
    vec3 C = BilinearSample(uSourceTex, uv + texelSize * vec2( 1.0, -1.0), wC);
    vec3 D = BilinearSample(uSourceTex, uv + texelSize * vec2(-0.5, -0.5), wD);
    vec3 E = BilinearSample(uSourceTex, uv + texelSize * vec2( 0.5, -0.5), wE);
    vec3 F = BilinearSample(uSourceTex, uv + texelSize * vec2(-1.0,  0.0), wF);
    vec3 G = BilinearSample(uSourceTex, uv + texelSize * vec2( 0.0,  0.0), wG);
    vec3 H = BilinearSample(uSourceTex, uv + texelSize * vec2( 1.0,  0.0), wH);
    vec3 I = BilinearSample(uSourceTex, uv + texelSize * vec2(-0.5,  0.5), wI);
    vec3 J = BilinearSample(uSourceTex, uv + texelSize * vec2( 0.5,  0.5), wJ);
    vec3 K = BilinearSample(uSourceTex, uv + texelSize * vec2(-1.0,  1.0), wK);
    vec3 L = BilinearSample(uSourceTex, uv + texelSize * vec2( 0.0,  1.0), wL);
    vec3 M = BilinearSample(uSourceTex, uv + texelSize * vec2( 1.0,  1.0), wM);

    vec3 c = vec3(0.0);
    c += PartialAverage(D, E, I, J, wD, wE, wI, wJ) * 0.5;
    c += PartialAverage(A, B, F, G, wA, wB, wF, wG) * 0.125;
    c += PartialAverage(B, C, G, H, wB, wC, wG, wH) * 0.125;
    c += PartialAverage(F, G, K, L, wF, wG, wK, wL) * 0.125;
    c += PartialAverage(G, H, L, M, wG, wH, wL, wM) * 0.125;

    OutColor = vec4(c, 1.0);
}