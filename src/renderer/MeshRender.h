#pragma once

#include "ptr.h"
#include "meshes/Mesh.h"
#include "base/Material.h"

class MeshRender
{
    SHARED_PTR(MeshRender)
public:
    MeshRender(Mesh::Ptr mesh, Material::Ptr mat);
    ~MeshRender() = default;

    Mesh::Ptr getMesh() { return m_Mesh; }
    Material::Ptr getMaterial() { return m_Material; }
private:
    Mesh::Ptr m_Mesh;
    Material::Ptr m_Material;
};