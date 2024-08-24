#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "ptr.h"
#include "renderer/MeshRender.h"

#include "utility/Collision.h"

using namespace Collision;

class SceneNode : public std::enable_shared_from_this<SceneNode>
{
    SHARED_PTR(SceneNode)
public:
    std::weak_ptr<SceneNode> getWeakPtr() {
        return shared_from_this();
    }

    SceneNode();
    ~SceneNode();

    glm::mat4 GetModelMatrix();

    void SetOverrideMaterial(Material::Ptr mat);

    void Translate(const glm::vec3 &p);
    void Rotate(const glm::vec3 &axis, const float &radians);
    void Scale(const glm::vec3 &scale);

    size_t GetChildrenCount();
    SceneNode::Ptr GetChildByIndex(size_t index);
    void AddChild(SceneNode::Ptr node);
    
    void MergeChildrenAABBs(BoundingBox &boundingBox);

    std::vector<MeshRender::Ptr> MeshRenders;
    glm::mat4 ModelMatrix;

    Material::Ptr OverrideMat;
    
    glm::mat4 Transform;

    bool IsAABBCalculated;
    BoundingBox AABB;

private:
    std::weak_ptr<SceneNode> m_Parent;
    std::vector<SceneNode::Ptr> m_Children;
};
