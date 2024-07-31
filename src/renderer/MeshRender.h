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
    MeshRender(Mesh *mesh, Shader* shaderProgram);
    ~MeshRender() = default;

    void addTexture(Texture* texture);

    void draw(Camera* camera);
private:
    Shader* m_Shader;
    std::vector<Texture*> m_Textures;

    Mesh* m_Mesh;
    Material* m_Material;
};