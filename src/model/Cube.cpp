#include "model/Cube.h"

std::vector<VertexPosition> cubeVertices = {
    VertexPosition(-0.5f, -0.5f, -0.5f),
    VertexPosition(0.5f, -0.5f, -0.5f),
    VertexPosition(0.5f,  0.5f, -0.5f),
    VertexPosition(0.5f,  0.5f, -0.5f),
    VertexPosition(-0.5f,  0.5f, -0.5f),
    VertexPosition(-0.5f, -0.5f, -0.5f),

    VertexPosition(-0.5f, -0.5f,  0.5f),
    VertexPosition(0.5f, -0.5f,  0.5f),
    VertexPosition(0.5f,  0.5f,  0.5f),
    VertexPosition(0.5f,  0.5f,  0.5f),
    VertexPosition(-0.5f,  0.5f,  0.5f),
    VertexPosition(-0.5f, -0.5f,  0.5f),

    VertexPosition(-0.5f,  0.5f,  0.5f),
    VertexPosition(-0.5f,  0.5f, -0.5f),
    VertexPosition(-0.5f, -0.5f, -0.5f),
    VertexPosition(-0.5f, -0.5f, -0.5f),
    VertexPosition(-0.5f, -0.5f,  0.5f),
    VertexPosition(-0.5f,  0.5f,  0.5f),

    VertexPosition(0.5f,  0.5f,  0.5f),
    VertexPosition(0.5f,  0.5f, -0.5f),
    VertexPosition(0.5f, -0.5f, -0.5f),
    VertexPosition(0.5f, -0.5f, -0.5f),
    VertexPosition(0.5f, -0.5f,  0.5f),
    VertexPosition(0.5f,  0.5f,  0.5f),

    VertexPosition(-0.5f, -0.5f, -0.5f),
    VertexPosition(0.5f, -0.5f, -0.5f),
    VertexPosition(0.5f, -0.5f,  0.5f),
    VertexPosition(0.5f, -0.5f,  0.5f),
    VertexPosition(-0.5f, -0.5f,  0.5f),
    VertexPosition(-0.5f, -0.5f, -0.5f),

    VertexPosition(-0.5f,  0.5f, -0.5f),
    VertexPosition(0.5f,  0.5f, -0.5f),
    VertexPosition(0.5f,  0.5f,  0.5f),
    VertexPosition(0.5f,  0.5f,  0.5f),
    VertexPosition(-0.5f,  0.5f,  0.5f),
    VertexPosition(-0.5f,  0.5f, -0.5f)
};

Cube::Cube() : Mesh(cubeVertices) {}