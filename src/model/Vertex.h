#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

using namespace glm;

struct VertexPosition
{
    vec3 position;

    VertexPosition(float x, float y, float z)
        : position(vec3(x, y, z)) { }

    VertexPosition(vec3 p)
        : position(p) { }
};
