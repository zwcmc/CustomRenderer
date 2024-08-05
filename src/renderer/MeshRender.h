#pragma once

#include <iostream>
#include <vector>

#include <memory>

#include "ptr.h"
#include "base/Material.h"
#include "meshes/Mesh.h"
#include "cameras/ArcballCamera.h"

class BaseLight;

class MeshRender
{
    SHARED_PTR(MeshRender)
public:
    MeshRender(Mesh::Ptr mesh, Material::Ptr mat);
    ~MeshRender() = default;

    Material::Ptr getMaterial() { return m_Material; }

    void draw(ArcballCamera::Ptr camera, glm::mat4 modelMatrix);
    void draw(ArcballCamera::Ptr camera, std::shared_ptr<BaseLight> light, glm::mat4 modelMatrix);

private:
    Mesh::Ptr m_Mesh;
    Material::Ptr m_Material;
};