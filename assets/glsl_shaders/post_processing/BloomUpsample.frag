#version 410 core
out vec4 OutColor;

in vec2 UV0;

uniform sampler2D uSourceTex;
uniform sampler2D uLowSourceTex;

uniform float uScatter;

void main()
{
    vec2 uv = UV0;

    vec3 highMip = texture(uSourceTex, uv).rgb;
    vec3 lowMip = texture(uLowSourceTex, uv).rgb;

    OutColor = vec4(mix(highMip, lowMip, uScatter), 1.0);
}