#pragma once

#include <glad/glad.h>

class Buffer
{
public:
    Buffer() : m_BufferID(0) { }
    virtual ~Buffer() = default;

    virtual void bind() = 0;
    virtual void unBind() = 0;

    GLuint getBufferID() const { return m_BufferID; }
protected:
    GLuint m_BufferID;
};