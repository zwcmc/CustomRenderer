#pragma once

#include "model/Mesh.h"
#include "ptr.h"

class Quad : public Mesh
{
    SHARED_PTR(Quad)
public:
    Quad();
    virtual ~Quad() = default;
};