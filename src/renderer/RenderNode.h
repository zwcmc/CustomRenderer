#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "ptr.h"
#include "renderer/MeshRender.h"

class RenderNode
{
    SHARED_PTR(RenderNode)
public:
    RenderNode();
    ~RenderNode();

    glm::mat4 getModelMatrix();

    void setOverrideMaterial(Material::Ptr mat);
    
    void translate(const glm::vec3 &p);
    void rotate(const glm::vec3 &axis, const float &radians);
    void scale(const glm::vec3 &scale);

    std::vector<MeshRender::Ptr> MeshRenders;
    glm::mat4 ModelMatrix;

    std::vector<RenderNode::Ptr> Children;
    std::weak_ptr<RenderNode> Parent;

    Material::Ptr OverrideMat;
    
    glm::mat4 Transform;

    glm::vec3 AABBMin;
    glm::vec3 AABBMax;
};
