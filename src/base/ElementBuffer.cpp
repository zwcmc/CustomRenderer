#include "base/ElementBuffer.h"

ElementBuffer::ElementBuffer(std::vector<GLuint> &indices) : Buffer()
{
    initBuffer(indices);
}

ElementBuffer::~ElementBuffer()
{
    unBind();
    glDeleteBuffers(1, &m_BufferID);
}

void ElementBuffer::initBuffer()
{
}

void ElementBuffer::initBuffer(std::vector<GLuint> &indiecs)
{
    glGenBuffers(1, &m_BufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indiecs.size() * sizeof(GLuint), &indiecs[0], GL_STATIC_DRAW);
}

void ElementBuffer::bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
}

void ElementBuffer::unBind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}