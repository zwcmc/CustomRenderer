#include "model/Quad.h"

std::vector<Vertex> quadVertices = {
    Vertex(0.5f, 0.5f, 0.0f, 1.0f, 1.0f),
    Vertex(0.5f, -0.5f, 0.0f, 1.0f, 0.0f),
    Vertex(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f),
    Vertex(-0.5f, 0.5f, 0.0f, 0.0f, 1.0f)
};

std::vector<GLuint> quadIndices = {
    0, 1, 3,
    1, 2, 3
};

Quad::Quad() : Mesh(quadVertices, quadIndices) {}