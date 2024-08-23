#pragma once

#include <glm/glm.hpp>

#include "ptr.h"

class BaseLight
{
    SHARED_PTR(BaseLight)
public:
    BaseLight();
    BaseLight(const glm::vec3 &position, const glm::vec3 &color);
    ~BaseLight() = default;

    void setPosition(const glm::vec3 &position) { m_Position  = position; }
    glm::vec3& getLightPosition() { return m_Position; }
    glm::vec3& getLightColor() { return m_Color; }

protected:
    glm::vec3 m_Position;
    glm::vec3 m_Color;
};
