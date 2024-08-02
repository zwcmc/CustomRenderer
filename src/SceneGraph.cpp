#include "SceneGraph.h"

#include <glm/gtc/type_ptr.hpp>

SceneGraph::SceneGraph()
    : m_GlobalUniformBufferID(0)
{
    init();
}

SceneGraph::~SceneGraph()
{
    m_ModelRenderers.clear();
    m_Lights.clear();
}

void SceneGraph::init()
{
    // Global uniform buffer object
    glGenBuffers(1, &m_GlobalUniformBufferID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_GlobalUniformBufferID);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_GlobalUniformBufferID); // Set global uniform to binding point 0
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void SceneGraph::setCamera(ArcballCamera::Ptr camera)
{
    m_Camera = camera;
}

void SceneGraph::rotateModelRenderers(const glm::vec3 &delta)
{
    for (auto &model : m_ModelRenderers)
    {
        model->rotate(glm::radians(delta.x) * ROTATION_SPEED, glm::vec3(1.0f, 0.0f, 0.0f));
        model->rotate(glm::radians(delta.y) * ROTATION_SPEED, glm::vec3(0.0f, 1.0f, 0.0f));
    }
}

void SceneGraph::addModelRenderer(ModelRenderer::Ptr renderer)
{
    m_ModelRenderers.push_back(renderer);
}

void SceneGraph::addLight(BaseLight::Ptr light)
{
    m_Lights.push_back(light);
}

void SceneGraph::render()
{
    glm::mat4 v = m_Camera->getViewMatrix();
    glm::mat4 p = m_Camera->getProjectionMatrix();

    glBindBuffer(GL_UNIFORM_BUFFER, m_GlobalUniformBufferID);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(v));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(p));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    if (m_Lights.size() > 0)
    {
        for (auto &light : m_Lights)
        {
            light->draw(m_Camera);

            for (auto &renderer : m_ModelRenderers)
            {
                renderer->draw(m_Camera, light);
            }
        }
    }
    else
    {
        for (auto &renderer : m_ModelRenderers)
        {
            renderer->draw(m_Camera);
        }
    }
}