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

    std::vector<MeshRender::Ptr> MeshRenders;
    glm::mat4 ModelMatrix;

    std::vector<RenderNode::Ptr> Children;
    std::weak_ptr<RenderNode> Parent;
};