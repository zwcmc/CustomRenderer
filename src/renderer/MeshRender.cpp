#include "renderer/MeshRender.h"

MeshRender::MeshRender(Mesh* mesh, ShaderProgram* shaderProgram)
    : m_Mesh(mesh), m_ShaderProgram(shaderProgram), m_Textures({})
{ }

void MeshRender::addTexture(Texture* texture)
{
    m_Textures.push_back(texture);
}

void MeshRender::draw(Camera* camera)
{
    // draw our first triangle
    m_ShaderProgram->useProgram();

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 p = camera->getProjectionMatrix();
    glm::mat4 v = camera->getViewMatrix();
    m_ShaderProgram->setUniform("model", model);
    m_ShaderProgram->setUniform("view", v);
    m_ShaderProgram->setUniform("projection", p);

    for (auto &texture : m_Textures)
    {
        m_ShaderProgram->setUniformi(texture->getTextureName(), texture->getTextureUnitIndex());
        texture->bind();
    }

    m_Mesh->draw();
}