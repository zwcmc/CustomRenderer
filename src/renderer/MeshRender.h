#pragma once

#include <iostream>
#include <vector>

#include "renderer/Camera.h"
#include "base/ShaderProgram.h"
#include "model/Mesh.h"
#include "base/texture/Texture.h"

class MeshRender
{
public:
    MeshRender(Mesh *mesh, ShaderProgram *shaderProgram);
    ~MeshRender() = default;

    void addTexture(Texture* texture);

    void draw(Camera* camera);
private:
    ShaderProgram* m_ShaderProgram;
    Mesh *m_Mesh;
    std::vector<Texture*> m_Textures;
};