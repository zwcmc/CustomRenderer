#pragma once

#include "ptr.h"
#include "meshes/Mesh.h"

class Sphere : public Mesh
{
    SHARED_PTR(Sphere)
public:
    Sphere(const unsigned int &xSegments, const unsigned int& ySegments, const float &scale = 1.0f);
    ~Sphere() = default;
};