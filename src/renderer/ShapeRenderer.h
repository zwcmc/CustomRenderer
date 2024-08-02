#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "ptr.h"
#include "renderer/ModelRenderer.h"
#include "renderer/MeshRender.h"

class ShapeRenderer : public ModelRenderer
{
    SHARED_PTR(ShapeRenderer)
public:

    ShapeRenderer();
    ShapeRenderer(Mesh::Ptr mesh, Material::Ptr mat);
    ~ShapeRenderer();

    void addMeshRender(MeshRender::Ptr meshRender);

    void translate(const glm::vec3 &p) override;
    void scale(const glm::vec3 &s) override;
    void rotate(const float &radians, const glm::vec3 &axis) override;

    void draw(ArcballCamera::Ptr camera) override;

private:
    glm::mat4 m_ModelMatrix;
    std::vector<MeshRender::Ptr> m_MeshRenders;
};