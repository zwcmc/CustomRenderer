#include "base/VertexArray.h"

VertexArray::VertexArray() : Buffer()
{
    initBuffer();
}

VertexArray::~VertexArray()
{
    unBind();
    glDeleteVertexArrays(1, &m_BufferID);
}

void VertexArray::initBuffer()
{
    glGenVertexArrays(1, &m_BufferID);
    glBindVertexArray(m_BufferID);
}

void VertexArray::bind()
{
    glBindVertexArray(m_BufferID);
}

void VertexArray::unBind()
{
    glBindVertexArray(0);
}