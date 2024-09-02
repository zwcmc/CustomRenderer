#version 410 core
out vec4 OutColor;

in vec2 UV0;

uniform sampler2D uSourceTex;

vec2 ClampUVForBilinear(vec2 uv, vec2 texelSize)
{
    // Assuming half a texel offset in the clamp.
    vec2 maxCoord = vec2(1.0) - 0.5 * texelSize;

    return min(uv, maxCoord);
}

void main()
{
    vec2 uv = UV0;
    vec2 size = textureSize(uSourceTex, 0);
    vec2 texelSize = (vec2(1.0) / size);

    // Optimized bilinear 5-tap gaussian on the same-sized source (9-tap equivalent)
    vec3 c0 = texture(uSourceTex, ClampUVForBilinear(uv - vec2(0.0, texelSize.y * 3.23076923), texelSize)).rgb;
    vec3 c1 = texture(uSourceTex, ClampUVForBilinear(uv - vec2(0.0, texelSize.y * 1.38461538), texelSize)).rgb;
    vec3 c2 = texture(uSourceTex, ClampUVForBilinear(uv                                      , texelSize)).rgb;
    vec3 c3 = texture(uSourceTex, ClampUVForBilinear(uv + vec2(0.0, texelSize.y * 1.38461538), texelSize)).rgb;
    vec3 c4 = texture(uSourceTex, ClampUVForBilinear(uv + vec2(0.0, texelSize.y * 3.23076923), texelSize)).rgb;

    vec3 color = c0 * 0.07027027 + c1 * 0.31621622
                + c2 * 0.22702703
                + c3 * 0.31621622 + c4 * 0.07027027;

    OutColor = vec4(color, 1.0);
}