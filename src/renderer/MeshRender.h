#pragma once

#include <iostream>
#include <vector>

#include "renderer/Camera.h"
#include "base/Shader.h"
#include "base/Texture.h"

#include "model/Mesh.h"
#include "base/Material.h"

class MeshRender
{
public:
    MeshRender(Mesh* mesh, Material* mat);
    ~MeshRender() = default;

    void draw(Camera* camera);
private:
    Mesh* m_Mesh;
    Material* m_Material;
};