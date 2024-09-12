#version 410 core
out vec4 FragColor;

in vec2 UV0;

uniform sampler2D uDepthTexture;

#include "common/constants.glsl"
#include "common/functions.glsl"

#define SAMPLE_COUNT 16.0
#define SPIRALTURNS 7.0
#define RADIUS 0.3
#define INTENSITY 3.0
#define BIAS 0.0005
#define POWER 2.0

/*
 * Random number between 0 and 1, using interleaved gradient noise.
 * w must not be normalized (e.g. window coordinates)
 */
float interleavedGradientNoise(vec2 w)
{
    const vec3 m = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(m.z * fract(dot(w, m.xy)));
}

vec2 startPosition(const float noise)
{
    float angle = ((2.0 * M_PI) * 2.4) * noise;
    return vec2(cos(angle), sin(angle));
}

mat2 tapAngleStep()
{
    float inc = (1.0 / (SAMPLE_COUNT - 0.5)) * SPIRALTURNS * M_TAU;
    vec2 t = vec2(cos(inc), sin(inc));
    return mat2(t.x, t.y, -t.y, t.x);
}

vec3 tapLocationFast(float i, vec2 p, const float noise)
{
    float sampleCountY = 1.0 / (SAMPLE_COUNT - 0.5);
    float radius = (i + noise + 0.5) * sampleCountY;
    return vec3(p, radius * radius);
}

float sampleDepthLinear(sampler2D depthTexture, vec2 uv)
{
    return LinearizeDepth(texture(depthTexture, uv).r);
}

vec3 computeViewSpacePositionFromDepth(vec2 uv, float linearDepth, vec2 positionParams)
{
    return vec3((0.5 - uv) * positionParams * linearDepth, linearDepth);
}

vec3 computeViewSpaceNormal(sampler2D depthTexture, vec2 uv, vec3 position, vec2 texel, vec2 positionParams)
{
    vec2 uvdx = uv + vec2(texel.x, 0.0);
    vec2 uvdy = uv + vec2(0.0, texel.y);
    vec3 px = computeViewSpacePositionFromDepth(uvdx, sampleDepthLinear(depthTexture, uvdx), positionParams);
    vec3 py = computeViewSpacePositionFromDepth(uvdy, sampleDepthLinear(depthTexture, uvdy), positionParams);
    vec3 dpdx = px - position;
    vec3 dpdy = py - position;
    return normalize(cross(dpdx, dpdy));
}

void computeAmbientOcclusionSAO(inout float occlusion, float i, float ssDiskRadius, vec2 uv, vec3 origin, vec3 normal, vec2 tapPosition, float noise)
{
    vec3 tap = tapLocationFast(i, tapPosition, noise);

    float ssRadius = max(1.0, tap.z * ssDiskRadius);

    vec2 size = textureSize(uDepthTexture, 0);
    vec2 uvSamplePos = uv + vec2(ssRadius * tap.xy) * (1.0 / size);

    float occlusionDepth = sampleDepthLinear(uDepthTexture, uvSamplePos);

    mat4 invProjection = inverse(ClipFromView);
    vec2 positionParams = vec2(invProjection[0][0], invProjection[1][1]) * 2.0;
    vec3 p = computeViewSpacePositionFromDepth(uvSamplePos, occlusionDepth, positionParams);

    vec3 v = p - origin;
    float vv = dot(v, v);
    float vn = dot(v, normal);

    float w = Sqr(max(0.0, 1.0 - vv * (1.0 / (RADIUS * RADIUS))));

    float minHorizonAngleSineSquared = pow(sin(0.0), 2.0);
    w *= step(vv * minHorizonAngleSineSquared, vn * vn);

    float sampleOcclusion = max(0.0, vn + (origin.z * BIAS)) / (vv + Sqr(0.1 * RADIUS));
    occlusion += w * sampleOcclusion;
}

void scalableAmbientObscurance(out float obscurance, vec2 uv, vec3 origin, vec3 normal)
{
    float noise = interleavedGradientNoise(gl_FragCoord.xy);
    vec2 tapPosition = startPosition(noise);
    mat2 angleStep = tapAngleStep();

    vec2 size = textureSize(uDepthTexture, 0);
    float projectionScale = min(0.5 * ClipFromView[0].x * size.x, 0.5 * ClipFromView[1].y * size.y);

    float ssDiskRadius = -(projectionScale * RADIUS / origin.z);

    obscurance = 0.0;
    for (float i = 0.0; i < SAMPLE_COUNT; i += 1.0)
    {
        computeAmbientOcclusionSAO(obscurance, i, ssDiskRadius, uv, origin, normal, tapPosition, noise);
        tapPosition = angleStep * tapPosition;
    }
    float intensity = 0.1 * RADIUS * M_TAU * INTENSITY / SAMPLE_COUNT;
    obscurance = sqrt(obscurance * intensity);
}

void main()
{
    vec2 uv = UV0;

    float z = texture(uDepthTexture, uv).r;
    z = LinearizeDepth(z);

    mat4 invProjection = inverse(ClipFromView);
    vec2 positionParams = vec2(invProjection[0][0], invProjection[1][1]) * 2.0;
    vec2 texel = 1.0 / textureSize(uDepthTexture, 0);
    vec3 origin = computeViewSpacePositionFromDepth(uv, z, positionParams);

    vec3 normal = computeViewSpaceNormal(uDepthTexture, uv, origin, texel, positionParams);

    float occlusion = 0.0;

    scalableAmbientObscurance(occlusion, uv, origin, normal);

    // occlusion
    occlusion = pow(clamp(occlusion, 0.0, 1.0), POWER);

    vec3 packedNormal = normal * 0.5 + 0.5;

    FragColor = vec4(occlusion, packedNormal);
}