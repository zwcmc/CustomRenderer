#include "renderer/SceneRenderer.h"

SceneRenderer::SceneRenderer()
    : m_Camera(nullptr)
{ }

SceneRenderer::~SceneRenderer()
{
    m_ModelRenderers.clear();
    m_Lights.clear();
}

void SceneRenderer::setCamera(Camera::Ptr camera)
{
    this->m_Camera = camera;
}

void SceneRenderer::updateCamera(const CameraUpdateType &moveType, const glm::vec3 &delta)
{
    if (!this->m_Camera)
    {
        std::cerr << "SceneRenderer: camera is nullptr" << std::endl;
        return;
    }
    switch (moveType)
    {
        case CameraUpdateType::POSITION:
        {
            this->m_Camera->translate(delta);
            break;
        }
        case CameraUpdateType::ASPECT_RATIO:
        {
            float aspect = delta.x / delta.y;
            this->m_Camera->setAspectRatio(aspect);
            break;
        }
        default:
            break;
    }
}

void SceneRenderer::rotateModelRenderers(const glm::vec3 &delta)
{
    for (auto &model : m_ModelRenderers)
    {
        model->rotate(glm::radians(delta.y) * 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
    }
}

void SceneRenderer::addModelRenderer(ModelRenderer::Ptr renderer)
{
    m_ModelRenderers.push_back(renderer);
}

void SceneRenderer::addLight(BaseLight::Ptr light)
{
    m_Lights.push_back(light);
}

void SceneRenderer::render()
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