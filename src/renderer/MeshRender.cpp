#include "renderer/MeshRender.h"

MeshRender::MeshRender(Mesh::Ptr mesh, Material::Ptr mat)
    : m_Mesh(mesh), m_Material(mat)
{ }

void MeshRender::draw(Camera::Ptr camera, glm::mat4 modelMatrix)
{
    glm::mat4 p = camera->getProjectionMatrix();
    glm::mat4 v = camera->getViewMatrix();

    m_Material->use();
    m_Material->setMatrix("model", modelMatrix);
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

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}