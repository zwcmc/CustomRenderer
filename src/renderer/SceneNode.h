#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "ptr.h"
#include "renderer/MeshRender.h"

class SceneNode : public std::enable_shared_from_this<SceneNode>
{
    SHARED_PTR(SceneNode)
public:
    std::weak_ptr<SceneNode> getWeakPtr() {
        return shared_from_this();
    }

    SceneNode();
    ~SceneNode();

    glm::mat4 getModelMatrix();

    void setOverrideMaterial(Material::Ptr mat);
    
    void translate(const glm::vec3 &p);
    void rotate(const glm::vec3 &axis, const float &radians);
    void scale(const glm::vec3 &scale);

    size_t getChildrenCount();
    SceneNode::Ptr getChildByIndex(size_t index);
    void addChild(SceneNode::Ptr node);

    std::vector<MeshRender::Ptr> MeshRenders;
    glm::mat4 ModelMatrix;

    Material::Ptr OverrideMat;
    
    glm::mat4 Transform;

    bool IsAABBCalculated;
    glm::vec3 AABBMin;
    glm::vec3 AABBMax;

private:
    std::weak_ptr<SceneNode> m_Parent;
    std::vector<SceneNode::Ptr> m_Children;
};
