#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

using namespace glm;

struct Vertex
{
    vec3 vertex;
    vec2 texcoord;

    Vertex(float x, float y, float z)
        : vertex(vec3(x, y, z)), texcoord(vec2(0.0f))
    { }

    Vertex(vec3 v)
        : vertex(v), texcoord(vec2(0.0f))
    { }

    Vertex(float x, float y, float z, float u, float v)
        : vertex(vec3(x, y, z)), texcoord(vec2(u, v))
    { }

    Vertex(vec3 v, vec2 uv)
        : vertex(v), texcoord(uv)
    { }
};
