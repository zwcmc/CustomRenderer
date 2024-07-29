#include "model/Mesh.h"

Mesh::Mesh(std::vector<VertexPosition> &vertices)
    : m_IsUseElementArrayBuffer(false), m_VertexLength(vertices.size()), m_IndicesLength(0)
{
    initMesh(vertices);
}

Mesh::Mesh(std::vector<VertexPosition> &vertices, std::vector<GLuint> &indices)
    : m_IsUseElementArrayBuffer(false), m_VertexLength(vertices.size()), m_IndicesLength(indices.size())
{
    initMesh(vertices, indices);
}

void Mesh::initMesh(std::vector<VertexPosition> &vertices)
{
    m_VertexArray = new VertexArray();
    m_VertexBuffer = new VertexBuffer(vertices);
    unBindBuffers();
}

void Mesh::initMesh(std::vector<VertexPosition> &vertices, std::vector<GLuint> &indices)
{
    m_IsUseElementArrayBuffer = !indices.empty();
    m_VertexArray = new VertexArray();
    m_VertexBuffer = m_IsUseElementArrayBuffer ? new VertexBuffer(vertices, indices) : new VertexBuffer(vertices);
    unBindBuffers();
}

void Mesh::bindBuffers()
{
    if (m_VertexArray != nullptr && m_VertexBuffer != nullptr)
        m_VertexArray->bind();
}

void Mesh::unBindBuffers()
{
    if (m_VertexArray != nullptr && m_VertexBuffer != nullptr)
        m_VertexArray->unBind();
}

void Mesh::draw()
{
    bindBuffers();

    if (m_IsUseElementArrayBuffer)
        glDrawElements(GL_TRIANGLES, m_IndicesLength, GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(GL_TRIANGLES, 0, m_VertexLength);

    unBindBuffers();
}