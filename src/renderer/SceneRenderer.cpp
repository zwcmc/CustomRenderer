#include "renderer/SceneRenderer.h"

SceneRenderer::SceneRenderer()
    : m_Camera(nullptr)
{ }

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
        case CameraUpdateType::ROTATION:
        {
            this->m_Camera->rotate(delta);
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

void SceneRenderer::addModelRenderer(ModelRenderer::Ptr renderer)
{
    m_ModelRenders.push_back(renderer);
}

void SceneRenderer::render()
{
    for (auto &renderer : m_ModelRenders)
    {
        renderer->draw(m_Camera);
    }
}