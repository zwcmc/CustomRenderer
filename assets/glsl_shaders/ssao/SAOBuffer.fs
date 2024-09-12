#version 410 core

out vec4 FragColor;

in vec2 UV0;

uniform sampler2D uNormalRoughness;
uniform sampler2D uPositionOcclusion;
uniform sampler2D uNoise;

uniform vec4 uKernels[64];

#include "common/uniforms.glsl"

#define SAMPLE_COUNT 16

void main()
{
    vec2 uv = UV0;

    float radius = 0.5;
    float bias = 0.025;

    vec2 renderSize = textureSize(uNormalRoughness, 0);
    vec2 noiseScale = renderSize.xy * vec2(1.0 / 4.0);

    vec3 fragPos = (ViewFromWorld * vec4(texture(uPositionOcclusion, uv).xyz, 1.0)).xyz;
    vec3 normal = mat3(ViewFromWorld) * texture(uNormalRoughness, uv).xyz;
    vec3 randomVec = texture(uNoise, uv * noiseScale).xyz;

    vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < SAMPLE_COUNT; ++i)
    {
        vec3 samplePoint = TBN * uKernels[i].xyz; // from tangent to view-space
        samplePoint = fragPos + samplePoint * radius; 

        // project samplePoint position (to samplePoint texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePoint, 1.0);
        offset = ClipFromView * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        // get samplePoint depth
        float sampleDepth = (ViewFromWorld * vec4(texture(uPositionOcclusion, offset.xy).xyz, 1.0)).z;

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePoint.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / float(SAMPLE_COUNT));
    occlusion = pow(occlusion, 0.8);

    FragColor = vec4(vec3(occlusion), 1.0);
}