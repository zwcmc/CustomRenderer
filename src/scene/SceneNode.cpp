#include "scene/SceneNode.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assert.h>

SceneNode::SceneNode()
    : ModelMatrix(glm::mat4(1.0f)), Transform(glm::mat4(1.0f)), AABBMin(glm::vec3(-FLT_MAX)), AABBMax(glm::vec3(FLT_MAX)), IsAABBCalculated(false)
{ }

SceneNode::~SceneNode()
{
    m_Children.clear();
}

glm::mat4 SceneNode::getModelMatrix()
{
    glm::mat4 model = ModelMatrix;
    SceneNode::Ptr currentParent = m_Parent.lock();
    while (currentParent)
    {
        model = currentParent->ModelMatrix * model;
        currentParent = currentParent->m_Parent.lock();
    }

    return model;
}

void SceneNode::setOverrideMaterial(Material::Ptr mat)
{
    OverrideMat = mat;
    for (size_t i = 0; i < m_Children.size(); ++i)
        m_Children[i]->setOverrideMaterial(mat);
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

void SceneNode::addChild(SceneNode::Ptr node)
{
    // Check if node already has a parent
    if (node->m_Parent.lock()) return;

    node->m_Parent = getWeakPtr();
    m_Children.push_back(node);
}

void SceneNode::mergeChildrenAABBs(glm::vec3 &min, glm::vec3 &max)
{
    if (IsAABBCalculated)
    {
        glm::vec4 aabbMin = getModelMatrix() * glm::vec4(AABBMin, 1.0f);
        glm::vec4 aabbMax = getModelMatrix() * glm::vec4(AABBMax, 1.0f);
        if (aabbMin.x < min.x) min.x = aabbMin.x;
        if (aabbMin.y < min.y) min.y = aabbMin.y;
        if (aabbMin.z < min.z) min.z = aabbMin.z;
        if (aabbMax.x > max.x) max.x = aabbMax.x;
        if (aabbMax.y > max.y) max.y = aabbMax.y;
        if (aabbMax.z > max.z) max.z = aabbMax.z;
    }

    for (size_t i = 0; i < m_Children.size(); ++i)
        m_Children[i]->mergeChildrenAABBs(min, max);
}

size_t SceneNode::getChildrenCount()
{
    return m_Children.size();
}

SceneNode::Ptr SceneNode::getChildByIndex(size_t index)
{
    assert(index < getChildrenCount());
    return m_Children[index];
}
