#pragma once

#include <vector>
#include <glm/mat4x4.hpp>

#include "ptr.h"
#include "model/Mesh.h"
#include "renderer/Camera.h"
#include "base/Material.h"

class glTFRenderer
{
    SHARED_PTR(glTFRenderer)
public:
    struct glTFNode
    {
        using Ptr = std::shared_ptr<glTFNode>;
        inline static Ptr New()
        {
            return std::make_shared<glTFNode>();
        }

        // Pointer to the parent node, using weak_ptr to avoid circular references
        std::weak_ptr<glTFNode> parent;
        std::vector<glTFNode::Ptr> children;
        std::vector<Mesh::Ptr> meshes;
        glm::mat4 matrix;

        glTFNode() : matrix(1.0f) { }
    };

    glTFRenderer();
    ~glTFRenderer();

    void addNode(glTFNode::Ptr node);
    void setMaterial(Material::Ptr mat);

    void draw(Camera::Ptr camera);

private:
    void drawNode(Camera::Ptr camera, glTFNode::Ptr node);

    std::vector<glTFNode::Ptr> m_glTFNodes;
    Material::Ptr m_Material;
};