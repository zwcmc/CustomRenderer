#version 410 core
out vec4 OutColor;

in vec2 UV0;

uniform sampler2D uSourceTex;
uniform vec4 uSourceTexelSize; // { x: 1.0 / width, y: 1.0 / height, z: width, w: height }

uniform float uBloomSet;
uniform sampler2D uBloomTex;

void main()
{
    vec2 uv = UV0;

    vec4 color = texture(uSourceTex, uv);

    vec3 bloom = vec3(0.0);
    if (uBloomSet > 0.0)
    {
        bloom = texture(uBloomTex, uv).rgb;
        color.rgb += bloom;
    }

    OutColor = color;
}