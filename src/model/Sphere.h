#pragma once

#include "ptr.h"
#include "model/Mesh.h"

class Sphere : public Mesh
{
    SHARED_PTR(Sphere)
public:
    Sphere(unsigned int xSegments, unsigned int ySegments);
    virtual ~Sphere() = default;
};