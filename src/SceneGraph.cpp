#include "SceneGraph.h"

SceneGraph::SceneGraph()
    : m_Camera(nullptr)
{ }

SceneGraph::~SceneGraph()
{
    m_ModelRenderers.clear();
    m_Lights.clear();
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
    if (m_Lights.size() > 0)
    {
        for (auto &light : m_Lights)
        {
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