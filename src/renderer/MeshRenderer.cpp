#include "renderer/MeshRenderer.h"

MeshRenderer::MeshRenderer(Mesh::Ptr mesh, Material::Ptr mat)
    : m_Mesh(mesh), m_Material(mat), m_Transform(glm::mat4(1.0f))
{ }

void MeshRenderer::translate(const glm::vec3 & position)
{
    m_Transform = glm::translate(m_Transform, position);
}

void MeshRenderer::scale(const glm::vec3 &scale)
{
    m_Transform = glm::scale(m_Transform, scale);
}

void MeshRenderer::rotate(const float &radians, const glm::vec3 &axis)
{
    m_Transform = glm::rotate(m_Transform, radians, axis);
}

void MeshRenderer::draw(Camera::Ptr camera)
{
    glm::mat4 p = camera->getProjectionMatrix();
    glm::mat4 v = camera->getViewMatrix();

    m_Material->use();
    m_Material->setMatrix("model", m_Transform);
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