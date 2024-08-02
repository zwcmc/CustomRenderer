#pragma once

#include "ptr.h"
#include "model/Mesh.h"

class Cube : public Mesh
{
    SHARED_PTR(Cube)
public:
    Cube();
    virtual ~Cube() = default;
};