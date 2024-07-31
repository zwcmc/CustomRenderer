#pragma once

#include <iostream>
#include <vector>

#include "renderer/Camera.h"
#include "model/Mesh.h"
#include "base/Material.h"

class MeshRender
{
public:
    MeshRender(Mesh* mesh, Material* mat);
    ~MeshRender() = default;

    void translate(const glm::vec3 &position);
    void scale(const glm::vec3 &scale);
    void rotate(const float &radians, const glm::vec3 &axis);

    void update();
    void draw(Camera* camera);
private:
    Mesh* m_Mesh;
    Material* m_Material;

    glm::mat4 m_Transform;
};