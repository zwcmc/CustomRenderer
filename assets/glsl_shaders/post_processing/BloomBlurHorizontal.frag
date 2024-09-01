#version 410 core
out vec4 OutColor;

in vec2 UV0;

uniform sampler2D uSourceTex;
uniform vec4 uSourceTexelSize;

vec2 ClampUVForBilinear(vec2 uv, vec2 texelSize)
{
    // Assuming half a texel offset in the clamp.
    vec2 maxCoord = vec2(1.0) - 0.5 * texelSize;

    return min(uv, maxCoord);
}

void main()
{
    vec2 texelSize = uSourceTexelSize.xy * 2.0;
    vec2 uv = UV0;

    // 9-tap gaussian blur on the downsampled source
    vec3 c0 = texture(uSourceTex, ClampUVForBilinear(uv - vec2(texelSize.x * 4.0, 0.0), texelSize)).rgb;
    vec3 c1 = texture(uSourceTex, ClampUVForBilinear(uv - vec2(texelSize.x * 3.0, 0.0), texelSize)).rgb;
    vec3 c2 = texture(uSourceTex, ClampUVForBilinear(uv - vec2(texelSize.x * 2.0, 0.0), texelSize)).rgb;
    vec3 c3 = texture(uSourceTex, ClampUVForBilinear(uv - vec2(texelSize.x * 1.0, 0.0), texelSize)).rgb;
    vec3 c4 = texture(uSourceTex, ClampUVForBilinear(uv                               , texelSize)).rgb;
    vec3 c5 = texture(uSourceTex, ClampUVForBilinear(uv + vec2(texelSize.x * 1.0, 0.0), texelSize)).rgb;
    vec3 c6 = texture(uSourceTex, ClampUVForBilinear(uv + vec2(texelSize.x * 2.0, 0.0), texelSize)).rgb;
    vec3 c7 = texture(uSourceTex, ClampUVForBilinear(uv + vec2(texelSize.x * 3.0, 0.0), texelSize)).rgb;
    vec3 c8 = texture(uSourceTex, ClampUVForBilinear(uv + vec2(texelSize.x * 4.0, 0.0), texelSize)).rgb;

    vec3 color = c0 * 0.01621622 + c1 * 0.05405405 + c2 * 0.12162162 + c3 * 0.19459459
                + c4 * 0.22702703
                + c5 * 0.19459459 + c6 * 0.12162162 + c7 * 0.05405405 + c8 * 0.01621622;

    OutColor = vec4(color, 1.0);
}