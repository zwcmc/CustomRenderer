#pragma once

#include "ptr.h"
#include "meshes/Mesh.h"

// The triangle that covers the Clip Space
class TriangleCS : public Mesh
{
    SHARED_PTR(TriangleCS)
public:
    TriangleCS();
    ~TriangleCS() = default;
};