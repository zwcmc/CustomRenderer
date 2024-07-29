#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "opengl/Buffer.h"

class ElementBuffer : public Buffer
{
public:
    ElementBuffer(std::vector<GLuint> &indices);
    ~ElementBuffer();

    void bind() override;
    void unBind() override;

protected:
    void initBuffer() override;
    void initBuffer(std::vector<GLuint> &indices);
};