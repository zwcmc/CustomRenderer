#pragma once

#include <iostream>

#include "Camera.h"

class Renderer
{
public:
    enum class CameraUpdateType
    {
        POSITION,
        ROTATION,
        ASPECT_RATIO
    };

    Renderer();
    ~Renderer() = default;

    void setCamera(Camera* camera);

    void updateCamera(CameraUpdateType moveType, glm::vec3& delta = glm::vec3(0.0f));
private:
    Camera* m_Camera;
};