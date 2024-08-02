#include "meshes/Cube.h"

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

Cube::Cube()
    : Mesh(cubeVertices)
{ }