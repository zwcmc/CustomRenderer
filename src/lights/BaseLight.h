#pragma once

#include <glm/glm.hpp>

#include "ptr.h"
#include "renderer/MeshRender.h"

class BaseLight
{
    SHARED_PTR(BaseLight)
public:
    BaseLight();
    BaseLight(const glm::vec3 &position, const glm::vec3 &color, const float &intensity);
    ~BaseLight() = default;

    void setPosition(const glm::vec3 &position) { m_Position  = position; }

    glm::vec4 getLightPosition() { return glm::vec4(m_Position, 1.0f); }
    glm::vec4 getLightColorAndIntensity() { return glm::vec4(m_Color, m_Intensity); }

protected:
    glm::vec3 m_Position;
    glm::vec3 m_Color;
    float m_Intensity;
};