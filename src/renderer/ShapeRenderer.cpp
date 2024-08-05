#include "renderer/ShapeRenderer.h"

ShapeRenderer::ShapeRenderer()
    : m_ModelMatrix(glm::mat4(1.0f))
{ }

ShapeRenderer::ShapeRenderer(Mesh::Ptr mesh, Material::Ptr mat)
    : m_ModelMatrix(glm::mat4(1.0f))
{
    m_MeshRenders.push_back(MeshRender::New(mesh, mat));
}

ShapeRenderer::~ShapeRenderer()
{
    m_MeshRenders.clear();
}

void ShapeRenderer::addMeshRender(MeshRender::Ptr meshRender)
{
    m_MeshRenders.push_back(meshRender);
}

void ShapeRenderer::translate(const glm::vec3 &p)
{
    m_ModelMatrix = glm::translate(m_ModelMatrix, p);
}

void ShapeRenderer::scale(const glm::vec3 &s)
{
    m_ModelMatrix = glm::scale(m_ModelMatrix, s);
}

void ShapeRenderer::rotate(const float &radians, const glm::vec3 &axis)
{
    m_ModelMatrix = glm::rotate(m_ModelMatrix, radians, axis);
}

void ShapeRenderer::draw(ArcballCamera::Ptr camera, Material::AlphaMode mode)
{
    if (m_MeshRenders.size() > 0)
    {
        for (auto meshRender : m_MeshRenders)
        {
            if (meshRender->getMaterial()->getAlphaMode() == mode)
            {
                meshRender->draw(camera, m_ModelMatrix);
            }
        }
    }
}