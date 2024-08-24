#include "scene/SceneNode.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assert.h>

SceneNode::SceneNode()
    : ModelMatrix(glm::mat4(1.0f)), Transform(glm::mat4(1.0f)), IsAABBCalculated(false)
{ }

SceneNode::~SceneNode()
{
    m_Children.clear();
}

glm::mat4 SceneNode::GetModelMatrix()
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

void SceneNode::SetOverrideMaterial(Material::Ptr mat)
{
    OverrideMat = mat;
    for (size_t i = 0; i < m_Children.size(); ++i)
        m_Children[i]->SetOverrideMaterial(mat);
}

void SceneNode::Translate(const glm::vec3 &p)
{
    ModelMatrix = glm::translate(ModelMatrix, p);
}

void SceneNode::Rotate(const glm::vec3 &axis, const float &radians)
{
    ModelMatrix = glm::rotate(ModelMatrix, radians, axis);
}

void SceneNode::Scale(const glm::vec3 &scale)
{
    ModelMatrix = glm::scale(ModelMatrix, scale);
}

void SceneNode::AddChild(SceneNode::Ptr node)
{
    // Check if node already has a parent
    if (node->m_Parent.lock()) return;

    node->m_Parent = getWeakPtr();
    m_Children.push_back(node);
}

void SceneNode::MergeChildrenAABBs(BoundingBox &boundingBox)
{
    if (IsAABBCalculated)
        boundingBox.MergeBoundingBox(AABB, GetModelMatrix());

    for (size_t i = 0; i < m_Children.size(); ++i)
        m_Children[i]->MergeChildrenAABBs(boundingBox);
}

size_t SceneNode::GetChildrenCount()
{
    return m_Children.size();
}

SceneNode::Ptr SceneNode::GetChildByIndex(size_t index)
{
    assert(index < GetChildrenCount());
    return m_Children[index];
}
