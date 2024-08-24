#pragma once

#include <glm/glm.hpp>

#include "ptr.h"

class Light
{
    SHARED_PTR(Light)
public:
    Light(const glm::vec3 &position, const glm::vec3 &color);
    ~Light() = default;

    glm::vec3& GetLightPosition() { return m_Position; }
    glm::vec3& GetLightColor() { return m_Color; }

private:
    glm::vec3 m_Position;
    glm::vec3 m_Color;
};
