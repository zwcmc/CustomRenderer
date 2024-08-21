#include "RenderNode.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

RenderNode::RenderNode()
    : ModelMatrix(glm::mat4(1.0f)), Transform(glm::mat4(1.0f))
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

void RenderNode::setOverrideMaterial(Material::Ptr mat)
{
    OverrideMat = mat;
    for (size_t i = 0; i < Children.size(); ++i)
    {
        Children[i]->setOverrideMaterial(mat);
    }
}

void RenderNode::translate(const glm::vec3 &p)
{
    ModelMatrix = glm::translate(ModelMatrix, p);
}

void RenderNode::rotate(const glm::vec3 &axis, const float &radians)
{
    ModelMatrix = glm::rotate(ModelMatrix, radians, axis);
}

void RenderNode::scale(const glm::vec3 &scale)
{
    ModelMatrix = glm::scale(ModelMatrix, scale);
}
