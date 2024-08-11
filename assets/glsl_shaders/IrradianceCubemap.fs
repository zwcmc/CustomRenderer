#version 410 core
out vec4 FragColor;

#include "common/Defines.glsl"

in vec3 UVW;

uniform samplerCube uEnvironmentCubemap;

const float deltaPhi = 2.0 * M_PI / 180.0;
const float deltaTheta = 0.5 * M_PI / 64.0;

void main()
{
    vec3 N = normalize(UVW);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up = cross(N, right);

    vec3 irradiance = vec3(0.0);
    uint sampleCount = 0u;
    for (float phi = 0.0; phi < M_TAU; phi += deltaPhi)
    {
        for (float theta = 0.0; theta < M_PI_2; theta += deltaTheta)
        {
            vec3 tempVec = cos(phi) * right + sin(phi) * up;
            vec3 sampleVector = cos(theta) * N + sin(theta) * tempVec;
            irradiance += texture(uEnvironmentCubemap, sampleVector).rgb * cos(theta) * sin(theta);
            sampleCount++;
        }
    }

    FragColor = vec4(M_PI * irradiance / float(sampleCount), 1.0);
}