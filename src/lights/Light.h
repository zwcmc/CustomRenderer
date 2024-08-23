#pragma once

#include <glm/glm.hpp>

#include "ptr.h"

class Light
{
    SHARED_PTR(Light)
public:
    Light(const glm::vec3 &position, const glm::vec3 &color);
    ~Light() = default;

    void setPosition(const glm::vec3 &position) { m_Position  = position; }
    glm::vec3& getLightPosition() { return m_Position; }
    glm::vec3& getLightColor() { return m_Color; }

protected:
    glm::vec3 m_Position;
    glm::vec3 m_Color;
};
