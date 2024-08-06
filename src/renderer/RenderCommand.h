#pragma once

#include <glm/glm.hpp>

#include "ptr.h"
#include "meshes/Mesh.h"
#include "base/Material.h"

struct RenderCommand
{
    SHARED_PTR_STRUCT(RenderCommand)

    Mesh::Ptr Mesh;
    Material::Ptr Material;
    glm::mat4 Transform;

    RenderCommand() : Transform(glm::mat4(1.0f)) { }
};