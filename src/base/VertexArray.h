#pragma once

#include "base/Buffer.h"

class VertexArray : public Buffer
{
public:
    VertexArray();
    ~VertexArray();

    void bind() override;
    void unBind() override;

protected:
    void initBuffer();
};