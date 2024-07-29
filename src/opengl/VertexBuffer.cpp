#include "opengl/VertexBuffer.h"

VertexBuffer::VertexBuffer(std::vector<VertexPosition> &vertices) : Buffer()
{
    initBuffer(vertices);
}

VertexBuffer::VertexBuffer(std::vector<VertexPosition> &vertices, std::vector<GLuint> &indices) : Buffer()
{
    initBuffer(vertices, indices);
}

VertexBuffer::~VertexBuffer()
{
    unBind();
    glDeleteBuffers(1, &m_BufferID);
}

void VertexBuffer::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
}

void VertexBuffer::unBind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::initBuffer()
{

}

void VertexBuffer::initBuffer(std::vector<VertexPosition> &vertices)
{
    const static GLint vertexSize = sizeof(VertexPosition);

    glGenBuffers(1, &m_BufferID);
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * vertexSize, &vertices[0], GL_STATIC_DRAW);

    enableAttribute(0, 3, vertexSize, NULL);
}

void VertexBuffer::initBuffer(std::vector<VertexPosition> &vertices, std::vector<GLuint> &indices)
{
    initBuffer(vertices);

    if (!indices.empty())
        m_ElementArrayBuffer = new ElementBuffer(indices);
}

    void VertexBuffer::enableAttribute(const GLuint &index, const GLint &size, const GLuint &offset, const GLvoid *data)
{
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, offset, data);
}