#include "renderer/Renderer.h"

Renderer::Renderer()
    : m_Camera(NULL) { }

void Renderer::setCamera(Camera* camera)
{
    this->m_Camera = camera;
}

void Renderer::updateCamera(CameraUpdateType moveType, const glm::vec3 &delta)
{
    if (!this->m_Camera)
    {
        std::cout << "Renderer: camera is NULL" << std::endl;
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