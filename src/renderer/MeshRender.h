#pragma once

#include <iostream>
#include <vector>

#include "renderer/Camera.h"
#include "model/Mesh.h"
#include "base/Material.h"

#include "ptr.h"

class MeshRender
{
SHARED_PTR(MeshRender)
public:
    MeshRender(Mesh::Ptr mesh, Material::Ptr mat);
    ~MeshRender() = default;

    void draw(Camera::Ptr camera, glm::mat4 modelMatrix);
public:
    Mesh::Ptr m_Mesh;
    Material::Ptr m_Material;
};