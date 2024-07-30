#include "model/Cube.h"

std::vector<vec3> cubeVertices = {
    vec3(-0.5f, -0.5f, -0.5f),
    vec3(0.5f, -0.5f, -0.5f),
    vec3(0.5f,  0.5f, -0.5f),
    vec3(0.5f,  0.5f, -0.5f),
    vec3(-0.5f,  0.5f, -0.5f),
    vec3(-0.5f, -0.5f, -0.5f),

    vec3(-0.5f, -0.5f,  0.5f),
    vec3(0.5f, -0.5f,  0.5f),
    vec3(0.5f,  0.5f,  0.5f),
    vec3(0.5f,  0.5f,  0.5f),
    vec3(-0.5f,  0.5f,  0.5f),
    vec3(-0.5f, -0.5f,  0.5f),

    vec3(-0.5f,  0.5f,  0.5f),
    vec3(-0.5f,  0.5f, -0.5f),
    vec3(-0.5f, -0.5f, -0.5f),
    vec3(-0.5f, -0.5f, -0.5f),
    vec3(-0.5f, -0.5f,  0.5f),
    vec3(-0.5f,  0.5f,  0.5f),

    vec3(0.5f,  0.5f,  0.5f),
    vec3(0.5f,  0.5f, -0.5f),
    vec3(0.5f, -0.5f, -0.5f),
    vec3(0.5f, -0.5f, -0.5f),
    vec3(0.5f, -0.5f,  0.5f),
    vec3(0.5f,  0.5f,  0.5f),

    vec3(-0.5f, -0.5f, -0.5f),
    vec3(0.5f, -0.5f, -0.5f),
    vec3(0.5f, -0.5f,  0.5f),
    vec3(0.5f, -0.5f,  0.5f),
    vec3(-0.5f, -0.5f,  0.5f),
    vec3(-0.5f, -0.5f, -0.5f),

    vec3(-0.5f,  0.5f, -0.5f),
    vec3(0.5f,  0.5f, -0.5f),
    vec3(0.5f,  0.5f,  0.5f),
    vec3(0.5f,  0.5f,  0.5f),
    vec3(-0.5f,  0.5f,  0.5f),
    vec3(-0.5f,  0.5f, -0.5f)
};

std::vector<vec2> cubeTexcoords =
{
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(1.0f, 1.0f),
    vec2(1.0f, 1.0f),
    vec2(0.0f, 1.0f),
    vec2(0.0f, 0.0f),

    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(1.0f, 1.0f),
    vec2(1.0f, 1.0f),
    vec2(0.0f, 1.0f),
    vec2(0.0f, 0.0f),

    vec2(1.0f, 0.0f),
    vec2(1.0f, 1.0f),
    vec2(0.0f, 1.0f),
    vec2(0.0f, 1.0f),
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),

    vec2(1.0f, 0.0f),
    vec2(1.0f, 1.0f),
    vec2(0.0f, 1.0f),
    vec2(0.0f, 1.0f),
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),

    vec2(0.0f, 1.0f),
    vec2(1.0f, 1.0f),
    vec2(1.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(0.0f, 0.0f),
    vec2(0.0f, 1.0f),

    vec2(0.0f, 1.0f),
    vec2(1.0f, 1.0f),
    vec2(1.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(0.0f, 0.0f),
    vec2(0.0f, 1.0f)
};

Cube::Cube()
    : Mesh(cubeVertices, cubeTexcoords)
{ }