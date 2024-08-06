#include "renderer/glTFRenderer.h"

glTFRenderer::glTFRenderer()
    : m_ModelMatrix(glm::mat4(1.0f)), m_GLDoubleSideState(false), m_GLAlphaMode(Material::AlphaMode::OPAQUE)
{ }

void glTFRenderer::addNode(glTFNode::Ptr node)
{
    m_glTFNodes.push_back(node);
}

void glTFRenderer::draw(ArcballCamera::Ptr camera, Material::AlphaMode mode)
{
    if (m_glTFNodes.size() > 0)
    {
        for (auto node : m_glTFNodes)
        {
            drawNode(camera, node, mode);
        }
    }
}

void glTFRenderer::draw(ArcballCamera::Ptr camera, BaseLight::Ptr light, Material::AlphaMode mode)
{
    if (m_glTFNodes.size() > 0)
    {
        for (auto node : m_glTFNodes)
        {
            drawNode(camera, light, node, mode);
        }
    }
}

void glTFRenderer::translate(const glm::vec3& p)
{
    m_ModelMatrix = glm::translate(m_ModelMatrix, p);
}

void glTFRenderer::scale(const glm::vec3& s)
{
    m_ModelMatrix = glm::scale(m_ModelMatrix, s);
}

void glTFRenderer::rotate(const float& radians, const glm::vec3& axis)
{
    m_ModelMatrix = glm::rotate(m_ModelMatrix, radians, axis);
}

void glTFRenderer::drawNode(ArcballCamera::Ptr camera, glTFNode::Ptr node, Material::AlphaMode mode)
{
    if (node->meshRenders.size() > 0)
    {
        glm::mat4 modelMatrix = node->matrix;
        glTFNode::Ptr currentParent = node->parent.lock();
        while (currentParent)
        {
            modelMatrix = currentParent->matrix * modelMatrix;
            currentParent = currentParent->parent.lock();
        }

        modelMatrix = m_ModelMatrix * modelMatrix;

        for (auto mr : node->meshRenders)
        {
            if (mr->getMaterial()->getAlphaMode() == mode)
            {
                // TODO - These should cache gl state in SceneRenderGraph
                setGLDoubleSidedState(mr->getMaterial()->getDoubleSided());
                setGLAlphaMode(mr->getMaterial()->getAlphaMode());

                mr->draw(camera, modelMatrix);
            }
        }
    }

    for (auto child : node->children)
    {
        drawNode(camera, child, mode);
    }
}

void glTFRenderer::drawNode(ArcballCamera::Ptr camera, BaseLight::Ptr light, glTFNode::Ptr node, Material::AlphaMode mode)
{
    if (node->meshRenders.size() > 0)
    {
        glm::mat4 modelMatrix = node->matrix;
        glTFNode::Ptr currentParent = node->parent.lock();
        while (currentParent)
        {
            modelMatrix = currentParent->matrix * modelMatrix;
            currentParent = currentParent->parent.lock();
        }

        modelMatrix = m_ModelMatrix * modelMatrix;

        for (auto mr : node->meshRenders)
        {
            if (mr->getMaterial()->getAlphaMode() == mode)
            {
                // TODO - These should cache gl state in SceneRenderGraph
                setGLDoubleSidedState(mr->getMaterial()->getDoubleSided());
                setGLAlphaMode(mr->getMaterial()->getAlphaMode());

                mr->draw(camera, light, modelMatrix);
            }
        }
    }

    for (auto child : node->children)
    {
        drawNode(camera, light, child, mode);
    }
}

void glTFRenderer::setGLDoubleSidedState(bool bDoubleSided)
{
    if (m_GLDoubleSideState != bDoubleSided)
    {
        m_GLDoubleSideState = bDoubleSided;

        if (bDoubleSided)
        {
            glDisable(GL_CULL_FACE);
        }
        else
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }
    }
}

void glTFRenderer::setGLAlphaMode(Material::AlphaMode mode)
{
    if (m_GLAlphaMode != mode)
    {
        m_GLAlphaMode = mode;
        if (mode == Material::AlphaMode::BLEND)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }
}