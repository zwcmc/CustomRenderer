#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "ptr.h"
#include "renderer/MeshRender.h"

class SceneNode
{
    SHARED_PTR(SceneNode)
public:
    SceneNode();
    ~SceneNode();

    glm::mat4 getModelMatrix();

    void setOverrideMaterial(Material::Ptr mat);
    
    void translate(const glm::vec3 &p);
    void rotate(const glm::vec3 &axis, const float &radians);
    void scale(const glm::vec3 &scale);

    std::vector<MeshRender::Ptr> MeshRenders;
    glm::mat4 ModelMatrix;

    std::vector<SceneNode::Ptr> Children;
    std::weak_ptr<SceneNode> Parent;

    Material::Ptr OverrideMat;
    
    glm::mat4 Transform;

    glm::vec3 AABBMin;
    glm::vec3 AABBMax;
};
