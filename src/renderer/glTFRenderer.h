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

    struct glTFMaterialData
    {
        SHARED_PTR_STRUCT(glTFMaterialData)

        Texture::Ptr baseColorTexture;
        glm::vec4 baseColorFactor;

        Texture::Ptr normalTexture;

        Texture::Ptr emissiveTexture;
        glm::vec3 emissiveFactor;

        Texture::Ptr metallicRoughnessTexture;
        float metallicFactor;
        float roughnessFactor;

        Texture::Ptr occlusionTexture;
        /*int alphaMode;
        float alphaCutoff;
        bool doubleSided;*/

        glTFMaterialData()
            : baseColorFactor(glm::vec4(1.0f)), emissiveFactor(glm::vec3(0.0f)), metallicFactor(1.0f), roughnessFactor(1.0f)
        { }
    };

    glTFRenderer();
    ~glTFRenderer() = default;

    void addNode(glTFNode::Ptr node);
    void draw(ArcballCamera::Ptr camera) override;
    void draw(ArcballCamera::Ptr camera, BaseLight::Ptr light) override;

    void translate(const glm::vec3 &p) override;
    void scale(const glm::vec3 &s) override;
    void rotate(const float &radians, const glm::vec3 &axis) override;

private:
    void drawNode(ArcballCamera::Ptr camera, glTFNode::Ptr node);
    void drawNode(ArcballCamera::Ptr camera, BaseLight::Ptr light, glTFNode::Ptr node);

    std::vector<glTFNode::Ptr> m_glTFNodes;

    glm::mat4 m_ModelMatrix;
};