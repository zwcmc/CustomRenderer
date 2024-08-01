#pragma once

#include <iostream>
#include <vector>

#include "ptr.h"

#include "renderer/Camera.h"
#include "model/Mesh.h"
#include "base/Material.h"
#include "lights/BaseLight.h"

class MeshRender
{
SHARED_PTR(MeshRender)
public:
    MeshRender(Mesh::Ptr mesh, Material::Ptr mat);
    ~MeshRender() = default;

    void draw(Camera::Ptr camera, glm::mat4 modelMatrix);
    void draw(Camera::Ptr camera, BaseLight::Ptr light, glm::mat4 modelMatrix);

public:
    Mesh::Ptr m_Mesh;
    Material::Ptr m_Material;
};