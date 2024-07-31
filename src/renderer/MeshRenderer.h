#pragma once

#include <iostream>
#include <vector>

#include "renderer/Camera.h"
#include "model/Mesh.h"
#include "base/Material.h"

#include "ptr.h"

class MeshRenderer
{
    SHARED_PTR(MeshRenderer)
public:
    MeshRenderer(Mesh::Ptr mesh, Material::Ptr mat);
    ~MeshRenderer() = default;

    void translate(const glm::vec3 &position);
    void scale(const glm::vec3 &scale);
    void rotate(const float &radians, const glm::vec3 &axis);

    void draw(Camera::Ptr camera);
private:
    Mesh::Ptr m_Mesh;
    Material::Ptr m_Material;

    glm::mat4 m_Transform;
};