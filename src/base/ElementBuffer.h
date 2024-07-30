#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "base/Buffer.h"

class ElementBuffer : public Buffer
{
public:
    ElementBuffer(std::vector<GLuint> &indices);
    ~ElementBuffer();

    void bind() override;
    void unBind() override;

protected:
    void initBuffer(std::vector<GLuint> &indices);
};