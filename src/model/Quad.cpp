#include "model/Quad.h"

std::vector<vec3> quadVertices =
{
    vec3(0.5f, 0.5f, 0.0f),
    vec3(0.5f, -0.5f, 0.0f),
    vec3(-0.5f, -0.5f, 0.0f),
    vec3(-0.5f, 0.5f, 0.0f)
};

std::vector<vec2> quadTexcoords = 
{
    vec2(1.0f, 1.0f),
    vec2(1.0f, 0.0f),
    vec2(0.0f, 0.0f),
    vec2(0.0f, 1.0f)
};

std::vector<unsigned int> quadIndices =
{
    0, 1, 3,
    1, 2, 3
};

Quad::Quad()
    : Mesh(quadVertices, quadTexcoords, quadIndices)
{ }