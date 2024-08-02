#include "renderer/MeshRender.h"

MeshRender::MeshRender(Mesh::Ptr mesh, Material::Ptr mat)
    : m_Mesh(mesh), m_Material(mat)
{ }

void MeshRender::draw(Camera::Ptr camera, glm::mat4 modelMatrix)
{
    glm::mat4 p = camera->getProjectionMatrix();
    glm::mat4 v = camera->getViewMatrix();

    m_Material->use();
    m_Material->setMatrix("uModelMatrix", modelMatrix);
    m_Material->setMatrix("uViewMatrix", v);
    m_Material->setMatrix("uProjectionMatrix", p);

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

void MeshRender::draw(Camera::Ptr camera, BaseLight::Ptr light, glm::mat4 modelMatrix)
{
    glm::mat4 p = camera->getProjectionMatrix();
    glm::mat4 v = camera->getViewMatrix();

    m_Material->addVectorProperty("uLightPos", light->getLightPosition());
    m_Material->addVectorProperty("uLightColorIntensity", light->getLightColorAndIntensity());

    m_Material->use();
    m_Material->setMatrix("uModelMatrix", modelMatrix);
    m_Material->setMatrix("uViewMatrix", v);
    m_Material->setMatrix("uProjectionMatrix", p);
    m_Material->setMatrix("uModelMatrixInverse", glm::mat3x3(glm::inverse(modelMatrix)));

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