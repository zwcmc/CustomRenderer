#include "RenderNode.h"

RenderNode::RenderNode()
    : ModelMatrix(glm::mat4(1.0f))
{ }

RenderNode::~RenderNode() { }

glm::mat4 RenderNode::getModelMatrix()
{
    glm::mat4 model = ModelMatrix;
    RenderNode::Ptr currentParent = Parent.lock();
    while (currentParent)
    {
        model = currentParent->ModelMatrix * model;
        currentParent = currentParent->Parent.lock();
    }

    return model;
}