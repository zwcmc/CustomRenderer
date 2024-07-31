#pragma once

#include <iostream>
#include <vector>

#include "renderer/Camera.h"
#include "base/Shader.h"
#include "model/Mesh.h"
#include "base/texture/Texture.h"

class MeshRender
{
public:
    MeshRender(Mesh *mesh, Shader* shaderProgram);
    ~MeshRender() = default;

    void addTexture(Texture* texture);

    void draw(Camera* camera);
private:
    Shader* m_Shader;
    Mesh *m_Mesh;
    std::vector<Texture*> m_Textures;
};