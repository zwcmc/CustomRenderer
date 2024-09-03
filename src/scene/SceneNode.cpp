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
        model = currentParent->Transform * currentParent->ModelMatrix * model;
        currentParent = currentParent->m_Parent.lock();
    }

    return Transform * model;
}

void SceneNode::SetOverrideMaterial(Material::Ptr mat)
{
    OverrideMat = mat;
    for (size_t i = 0; i < m_Children.size(); ++i)
        m_Children[i]->SetOverrideMaterial(mat);
}

void SceneNode::Translate(const glm::vec3 &p)
{
    Transform = glm::translate(Transform, p);
}

void SceneNode::Rotate(const glm::vec3 &axis, const float &radians)
{
    Transform = glm::rotate(Transform, radians, axis);
}

void SceneNode::Scale(const glm::vec3 &scale)
{
    Transform = glm::scale(Transform, scale);
}

void SceneNode::AddChild(SceneNode::Ptr node)
{
    // Check if node already has a parent
    if (node->m_Parent.lock()) return;

    node->m_Parent = getWeakPtr();
    m_Children.push_back(node);
}

void SceneNode::MergeChildrenAABBs(BoundingBox &boundingBox, bool &firstMerge)
{
    if (IsAABBCalculated)
    {
        if (firstMerge)
        {
            firstMerge = !firstMerge;
            
            BoundingBox::CreateFromBoundingBoxAndTransform(boundingBox, AABB, GetModelMatrix());

//            vec3 bCenter = AABB.Center;
//            vec3 bExtents = AABB.Extents;
//            vec3 bMin = bCenter - bExtents;
//            vec3 bMax = bCenter + bExtents;
//            bMin = glm::make_vec3(GetModelMatrix() * vec4(bMin, 1.0f));
//            bMax = glm::make_vec3(GetModelMatrix() * vec4(bMax, 1.0f));
//
//            BoundingBox::CreateFromPoints(boundingBox, bMin, bMax);
        }
        else
        {
            boundingBox.MergeBoundingBox(AABB, GetModelMatrix());
        }
    }

    for (size_t i = 0; i < m_Children.size(); ++i)
        m_Children[i]->MergeChildrenAABBs(boundingBox, firstMerge);
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
