#pragma once

#include <vector>
#include <glm/mat4x4.hpp>

#include "ptr.h"
#include "renderer/ModelRenderer.h"
#include "renderer/MeshRender.h"

class glTFRenderer : public ModelRenderer
{
    SHARED_PTR(glTFRenderer)
public:
    struct glTFNode
    {
        SHARED_PTR_STRUCT(glTFNode)

        // Pointer to the parent node, using weak_ptr to avoid circular references
        std::weak_ptr<glTFNode> parent;
        std::vector<glTFNode::Ptr> children;
        std::vector<MeshRender::Ptr> meshRenders;
        glm::mat4 matrix;

        glTFNode() : matrix(1.0f) { }
    };

    glTFRenderer();
    ~glTFRenderer() = default;

    void addNode(glTFNode::Ptr node);
    void draw(ArcballCamera::Ptr camera, Material::AlphaMode mode) override;
    void draw(ArcballCamera::Ptr camera, BaseLight::Ptr light, Material::AlphaMode mode) override;

    void translate(const glm::vec3 &p) override;
    void scale(const glm::vec3 &s) override;
    void rotate(const float &radians, const glm::vec3 &axis) override;

private:
    void drawNode(ArcballCamera::Ptr camera, glTFNode::Ptr node, Material::AlphaMode mode);
    void drawNode(ArcballCamera::Ptr camera, BaseLight::Ptr light, glTFNode::Ptr node, Material::AlphaMode mode);

    void setGLDoubleSidedState(bool bDoubleSided);
    void setGLAlphaMode(Material::AlphaMode mode);

    std::vector<glTFNode::Ptr> m_glTFNodes;
    glm::mat4 m_ModelMatrix;

    bool m_GLDoubleSideState;
    Material::AlphaMode m_GLAlphaMode;
};