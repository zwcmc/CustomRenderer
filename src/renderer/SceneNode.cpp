#include "SceneNode.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

SceneNode::SceneNode()
    : ModelMatrix(glm::mat4(1.0f)), Transform(glm::mat4(1.0f)), AABBMin(glm::vec3(-99999.0f)), AABBMax(glm::vec3(99999.0f))
{ }

SceneNode::~SceneNode() { }

glm::mat4 SceneNode::getModelMatrix()
{
    glm::mat4 model = ModelMatrix;
    SceneNode::Ptr currentParent = Parent.lock();
    while (currentParent)
    {
        model = currentParent->ModelMatrix * model;
        currentParent = currentParent->Parent.lock();
    }

    return model;
}

void SceneNode::setOverrideMaterial(Material::Ptr mat)
{
    OverrideMat = mat;
    for (size_t i = 0; i < Children.size(); ++i)
        Children[i]->setOverrideMaterial(mat);
}

void SceneNode::translate(const glm::vec3 &p)
{
    ModelMatrix = glm::translate(ModelMatrix, p);
}

void SceneNode::rotate(const glm::vec3 &axis, const float &radians)
{
    ModelMatrix = glm::rotate(ModelMatrix, radians, axis);
}

void SceneNode::scale(const glm::vec3 &scale)
{
    ModelMatrix = glm::scale(ModelMatrix, scale);
}
