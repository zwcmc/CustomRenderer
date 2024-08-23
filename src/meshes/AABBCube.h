#pragma once

#include <glm/glm.hpp>

#include "ptr.h"
#include "meshes/Mesh.h"

class AABBCube : public Mesh
{
    SHARED_PTR(AABBCube);
public:
    AABBCube(const glm::vec3 &aabbMin, const glm::vec3 &aabbMax);
    ~AABBCube() = default;
};