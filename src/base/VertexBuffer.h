#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "base/ElementBuffer.h"
#include "model/Vertex.h"

class VertexBuffer : public Buffer
{
public:
    VertexBuffer(std::vector<Vertex> &vertices);
    VertexBuffer(std::vector<Vertex> &vertices, std::vector<GLuint> &indices);
    ~VertexBuffer();

    void bind() override;
    void unBind() override;

protected:
    void initBuffer(std::vector<Vertex> &vertices);
    void initBuffer(std::vector<Vertex> &vertices, std::vector<GLuint> &indices);

private:
    void enableAttribute(const GLuint &index, const GLint &size, const GLuint &offset, const GLvoid* data);

    ElementBuffer* m_ElementArrayBuffer;
};