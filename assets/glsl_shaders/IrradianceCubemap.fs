#version 410 core
out vec4 FragColor;

#include "common/constants.glsl"

in vec3 UVW;

uniform samplerCube uEnvironmentCubemap;

void main()
{
    vec3 N = normalize(UVW);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, N);
    up = cross(N, right);

    vec3 irradiance = vec3(0.0);
    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for (float phi = 0.0; phi < M_TAU; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < M_PI_2; theta += sampleDelta)
        {
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

            irradiance += texture(uEnvironmentCubemap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    irradiance = M_PI * irradiance * (1.0 / nrSamples);

    FragColor = vec4(irradiance, 1.0);
}