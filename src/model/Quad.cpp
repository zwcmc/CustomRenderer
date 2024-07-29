#include "model/Quad.h"

std::vector<VertexPosition> quadVertices = {
    VertexPosition(0.5f, 0.5f, 0.0f),
    VertexPosition(0.5f, -0.5f, 0.0f),
    VertexPosition(-0.5f, -0.5f, 0.0f),
    VertexPosition(-0.5f, 0.5f, 0.0f)
};

std::vector<GLuint> quadIndices = {
    0, 1, 3,
    1, 2, 3
};

Quad::Quad() : Mesh(quadVertices, quadIndices) {}