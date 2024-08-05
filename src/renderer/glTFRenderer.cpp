#include "renderer/glTFRenderer.h"

glTFRenderer::glTFRenderer()
    : m_ModelMatrix(glm::mat4(1.0f)), m_GLDoubleSideState(false)
{ }

void glTFRenderer::addNode(glTFNode::Ptr node)
{
    m_glTFNodes.push_back(node);
}

void glTFRenderer::draw(ArcballCamera::Ptr camera)
{
    if (m_glTFNodes.size() > 0)
    {
        for (auto node : m_glTFNodes)
        {
            drawNode(camera, node);
        }
    }
}

void glTFRenderer::draw(ArcballCamera::Ptr camera, BaseLight::Ptr light)
{
    if (m_glTFNodes.size() > 0)
    {
        for (auto node : m_glTFNodes)
        {
            drawNode(camera, light, node);
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

void glTFRenderer::drawNode(ArcballCamera::Ptr camera, glTFNode::Ptr node)
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
            // TODO - Should cache gl state in SceneGraph
            setGLDoubleSidedState(mr->getMaterial()->getDoubleSided());
            mr->draw(camera, modelMatrix);
        }
    }

    for (auto child : node->children)
    {
        drawNode(camera, child);
    }
}

void glTFRenderer::drawNode(ArcballCamera::Ptr camera, BaseLight::Ptr light, glTFNode::Ptr node)
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
            // TODO - Should cache gl state in SceneGraph
            setGLDoubleSidedState(mr->getMaterial()->getDoubleSided());
            mr->draw(camera, light, modelMatrix);
        }
    }

    for (auto child : node->children)
    {
        drawNode(camera, light, child);
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