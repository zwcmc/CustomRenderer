#include "renderer/MeshRender.h"

#include "lights/BaseLight.h"

MeshRender::MeshRender(Mesh::Ptr mesh, Material::Ptr mat)
    : m_Mesh(mesh), m_Material(mat)
{ }

void MeshRender::draw(ArcballCamera::Ptr camera, glm::mat4 modelMatrix)
{
    m_Material->use();
    m_Material->setMatrix("uModelMatrix", modelMatrix);
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

void MeshRender::draw(ArcballCamera::Ptr camera, BaseLight::Ptr light, glm::mat4 modelMatrix)
{
    m_Material->addVectorProperty("uLightDirection", light->getLightPosition());
    m_Material->addVectorProperty("uLightColorIntensity", light->getLightColorAndIntensity());
    m_Material->addVectorProperty("uCameraPos", camera->getPosition());

    m_Material->use();
    m_Material->setMatrix("uModelMatrix", modelMatrix);
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