#version 410 core
out vec4 FragColor;

#include "common/constants.glsl"

in vec3 UVW;

uniform samplerCube uEnvironmentCubemap;

void main()
{
    // 构建转换到立方体空间的 TBN 矩阵
    vec3 N = normalize(UVW);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up = normalize(cross(N, right));

    vec3 irradiance = vec3(0.0);
    float sampleDelta = 0.025;
    float nrSamples = 0.0;

    // 半球均匀采样
    // pdf = 1 / (2 \pi * 0.5 \pi)
    for (float phi = 0.0; phi < M_TAU; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < M_PI_2; theta += sampleDelta)
        {
            // 球面坐标转换为笛卡尔坐标
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // 将笛卡尔坐标转换到立方体空间
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
            // 采样环境 cubemap , 预积分 irradiance map
            irradiance += texture(uEnvironmentCubemap, sampleVec).rgb * cos(theta) * sin(theta);

            nrSamples++;
        }
    }

    irradiance = M_PI * irradiance * (1.0 / nrSamples);

    FragColor = vec4(irradiance, 1.0);
}