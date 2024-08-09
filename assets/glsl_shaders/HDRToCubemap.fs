#version 410 core
out vec4 FragColor;

uniform sampler2D uEnvMap;

in vec3 WorldPos;

const vec2 invAtan = vec2(0.1591,0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(WorldPos));
	vec3 color = texture(uEnvMap, uv).rgb;
    FragColor = vec4(vec3(color), 1.0);
}