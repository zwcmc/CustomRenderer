#include "renderer/glTFRenderer.h"

glTFRenderer::glTFRenderer() { }

glTFRenderer::~glTFRenderer() { }

void glTFRenderer::addNode(glTFNode::Ptr node)
{
    m_glTFNodes.push_back(node);
}

void glTFRenderer::setMaterial(Material::Ptr mat)
{
    m_Material = mat;
}

void glTFRenderer::draw(Camera::Ptr camera)
{
    if (m_glTFNodes.size() > 0)
    {
        for (auto node : m_glTFNodes)
        {
            drawNode(camera, node);
        }
    }
}

void glTFRenderer::drawNode(Camera::Ptr camera, glTFNode::Ptr node)
{
    if (node->meshes.size() > 0)
    {
        glm::mat4 modelMatrix = node->matrix;
        glTFNode::Ptr currentParent = node->parent.lock();
        while (currentParent)
        {
            modelMatrix = currentParent->matrix * modelMatrix;
            currentParent = currentParent->parent.lock();
        }

        for (auto mesh : node->meshes)
        {
            glm::mat4 p = camera->getProjectionMatrix();
            glm::mat4 v = camera->getViewMatrix();
            m_Material->use();
            m_Material->setMatrix("model", modelMatrix);
            m_Material->setMatrix("view", v);
            m_Material->setMatrix("projection", p);

            glBindVertexArray(mesh->getVertexArrayID());

            if (mesh->getIndicesCount() > 0)
            {
                glDrawElements(GL_TRIANGLES, mesh->getIndicesCount(), GL_UNSIGNED_INT, nullptr);
            }
            else
            {
                glDrawArrays(GL_TRIANGLES, 0, mesh->getVerticesCount());
            }

            glBindVertexArray(0);
        }
    }

    for (auto child : node->children)
    {
        drawNode(camera, child);
    }
}