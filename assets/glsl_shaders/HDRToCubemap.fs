#version 410 core
out vec4 FragColor;

uniform sampler2D uHDRMap;

in vec3 UVW;

const vec2 invAtan = vec2(0.1591, 0.3183); // invAtan.x = 1 / 2π, invAtan.y = 1 / π
vec2 SampleSphericalMap(vec3 uvw)
{
    // Convert Cartesian coordinates to spherical coordinates
    vec2 uv = vec2(atan(uvw.z, uvw.x), asin(uvw.y));

    // Convert uv.x(θ) from [-π, π] to [-0.5, 0.5], uv.y(ϕ) from [-π/2, π/2] to [-0.5, 0.5]
    uv *= invAtan;

    // Convert uv.x and uv.y to texture uv coordinates [0, 1]
    uv += 0.5;

    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(UVW));
    vec3 color = texture(uHDRMap, uv).rgb;
    FragColor = vec4(vec3(color), 1.0);
}