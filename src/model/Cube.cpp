#include "model/Cube.h"

std::vector<Vertex> cubeVertices = {
    Vertex(-0.5f, -0.5f, -0.5f),
    Vertex(0.5f, -0.5f, -0.5f),
    Vertex(0.5f,  0.5f, -0.5f),
    Vertex(0.5f,  0.5f, -0.5f),
    Vertex(-0.5f,  0.5f, -0.5f),
    Vertex(-0.5f, -0.5f, -0.5f),

    Vertex(-0.5f, -0.5f,  0.5f),
    Vertex(0.5f, -0.5f,  0.5f),
    Vertex(0.5f,  0.5f,  0.5f),
    Vertex(0.5f,  0.5f,  0.5f),
    Vertex(-0.5f,  0.5f,  0.5f),
    Vertex(-0.5f, -0.5f,  0.5f),

    Vertex(-0.5f,  0.5f,  0.5f),
    Vertex(-0.5f,  0.5f, -0.5f),
    Vertex(-0.5f, -0.5f, -0.5f),
    Vertex(-0.5f, -0.5f, -0.5f),
    Vertex(-0.5f, -0.5f,  0.5f),
    Vertex(-0.5f,  0.5f,  0.5f),

    Vertex(0.5f,  0.5f,  0.5f),
    Vertex(0.5f,  0.5f, -0.5f),
    Vertex(0.5f, -0.5f, -0.5f),
    Vertex(0.5f, -0.5f, -0.5f),
    Vertex(0.5f, -0.5f,  0.5f),
    Vertex(0.5f,  0.5f,  0.5f),

    Vertex(-0.5f, -0.5f, -0.5f),
    Vertex(0.5f, -0.5f, -0.5f),
    Vertex(0.5f, -0.5f,  0.5f),
    Vertex(0.5f, -0.5f,  0.5f),
    Vertex(-0.5f, -0.5f,  0.5f),
    Vertex(-0.5f, -0.5f, -0.5f),

    Vertex(-0.5f,  0.5f, -0.5f),
    Vertex(0.5f,  0.5f, -0.5f),
    Vertex(0.5f,  0.5f,  0.5f),
    Vertex(0.5f,  0.5f,  0.5f),
    Vertex(-0.5f,  0.5f,  0.5f),
    Vertex(-0.5f,  0.5f, -0.5f)
};

Cube::Cube() : Mesh(cubeVertices) {}