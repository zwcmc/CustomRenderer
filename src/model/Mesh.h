#pragma once

#include <iostream>
#include <vector>

#include "model/Vertex.h"

#include "base/VertexArray.h"
#include "base/VertexBuffer.h"
#include "base/texture/Texture.h"
#include "base/ShaderProgram.h"

class Mesh
{
public:
    Mesh(std::vector<Vertex> &vertices);
    Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices);
    virtual ~Mesh() = default;

    void draw();
protected:
    VertexArray* m_VertexArray;
    VertexBuffer* m_VertexBuffer;
private:
    void initMesh(std::vector<Vertex> &vertices);
    void initMesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices);
    void bindBuffers();
    void unBindBuffers();

    bool m_IsUseElementArrayBuffer;
    GLuint m_VertexLength, m_IndicesLength;
};