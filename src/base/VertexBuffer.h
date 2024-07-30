#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "base/ElementBuffer.h"
#include "model/Vertex.h"

class VertexBuffer : public Buffer
{
public:
    VertexBuffer(std::vector<VertexPosition> &vertices);
    VertexBuffer(std::vector<VertexPosition> &vertices, std::vector<GLuint> &indices);
    ~VertexBuffer();

    void bind() override;
    void unBind() override;

protected:
    void initBuffer() override;
    void initBuffer(std::vector<VertexPosition> &vertices);
    void initBuffer(std::vector<VertexPosition> &vertices, std::vector<GLuint> &indices);

private:
    void enableAttribute(const GLuint &index, const GLint &size, const GLuint &offset, const GLvoid* data);

    ElementBuffer* m_ElementArrayBuffer;
};