#pragma once

#include "model/Mesh.h"
#include "ptr.h"

class Cube : public Mesh
{
SHARED_PTR(Cube)
public:
    Cube();
    virtual ~Cube() = default;
};