#pragma once

#include "ptr.h"
#include "model/Mesh.h"

class Quad : public Mesh
{
    SHARED_PTR(Quad)
public:
    Quad();
    virtual ~Quad() = default;
};