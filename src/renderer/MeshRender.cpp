#include "renderer/MeshRender.h"

MeshRender::MeshRender(Mesh* mesh, Material* mat)
    : m_Mesh(mesh), m_Material(mat)
{ }

void MeshRender::draw(Camera* camera)
{
    /*m_Shader->use();

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 p = camera->getProjectionMatrix();
    glm::mat4 v = camera->getViewMatrix();
    m_Shader->setMatrix("model", model);
    m_Shader->setMatrix("view", v);
    m_Shader->setMatrix("projection", p);

    for (auto &texture : m_Textures)
    {
        m_Shader->setInt(texture->getTextureName(), texture->getTextureUnitIndex());
        texture->bind();
    }*/

    glm::mat4 m = glm::mat4(1.0f);
    glm::mat4 p = camera->getProjectionMatrix();
    glm::mat4 v = camera->getViewMatrix();

    m_Material->use();
    m_Material->setMatrix("model", m);
    m_Material->setMatrix("view", v);
    m_Material->setMatrix("projection", p);

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