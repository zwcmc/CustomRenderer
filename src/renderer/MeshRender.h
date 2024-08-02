#pragma once

#include <iostream>
#include <vector>

#include "ptr.h"
#include "base/Material.h"
#include "lights/BaseLight.h"
#include "model/Mesh.h"
#include "cameras/ArcballCamera.h"

class MeshRender
{
    SHARED_PTR(MeshRender)
public:
    MeshRender(Mesh::Ptr mesh, Material::Ptr mat);
    ~MeshRender() = default;

    void draw(ArcballCamera::Ptr camera, glm::mat4 modelMatrix);
    void draw(ArcballCamera::Ptr camera, BaseLight::Ptr light, glm::mat4 modelMatrix);

public:
    Mesh::Ptr m_Mesh;
    Material::Ptr m_Material;
};