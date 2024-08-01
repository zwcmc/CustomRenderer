#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "ptr.h"
#include "renderer/ModelRenderer.h"
#include "renderer/Camera.h"
#include "renderer/MeshRender.h"

class ShapeRenderer : public ModelRenderer
{
    SHARED_PTR(ShapeRenderer)
public:

    ShapeRenderer();
    ShapeRenderer(Mesh::Ptr mesh, Material::Ptr mat);
    ~ShapeRenderer();

    void addMeshRender(MeshRender::Ptr meshRender);
    void translate(const glm::vec3 &p);
    void scale(const glm::vec3 &s);
    void rotate(const float &radians, const glm::vec3 &axis);

    void draw(Camera::Ptr camera) override;

private:
    glm::mat4 m_ModelMatrix;
    std::vector<MeshRender::Ptr> m_MeshRenders;
};