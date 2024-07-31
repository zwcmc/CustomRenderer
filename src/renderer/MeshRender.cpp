#include "renderer/MeshRender.h"

MeshRender::MeshRender(Mesh *mesh, Shader* shader)
    : m_Mesh(mesh), m_Shader(shader), m_Textures({})
{ }

void MeshRender::addTexture(Texture* texture)
{
    m_Textures.push_back(texture);
}

void MeshRender::draw(Camera* camera)
{
    m_Shader->use();

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 p = camera->getProjectionMatrix();
    glm::mat4 v = camera->getViewMatrix();
    m_Shader->setUniform("model", model);
    m_Shader->setUniform("view", v);
    m_Shader->setUniform("projection", p);

    for (auto &texture : m_Textures)
    {
        m_Shader->setUniformi(texture->getTextureName(), texture->getTextureUnitIndex());
        texture->bind();
    }

    glBindVertexArray(m_Mesh->getVertexArrayID());

    if (m_Mesh->getIndicesCount() > 0)
    {
        glDrawElements(GL_TRIANGLES, m_Mesh->getIndicesCount(), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, m_Mesh->getVerticesCount());
    }

    glBindVertexArray(0);
}