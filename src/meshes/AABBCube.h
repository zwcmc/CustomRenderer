#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "ptr.h"
#include "meshes/Mesh.h"

class AABBCube : public Mesh
{
    SHARED_PTR(AABBCube);
public:
    AABBCube(const std::vector<glm::vec3> &vertices);
    ~AABBCube() = default;
};
