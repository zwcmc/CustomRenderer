#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "ptr.h"

// Forward declaration
class ShapeRenderer;
class ArcballCamera;

class BaseLight
{
    SHARED_PTR(BaseLight)
public:
    BaseLight();
    BaseLight(const glm::vec3 &position, const glm::vec3 &color, const float &intensity);
    ~BaseLight() = default;

    void draw(std::shared_ptr<ArcballCamera> camera);

    void setPosition(const glm::vec3 &position) { m_Position  = position; }
    glm::vec4 getLightPosition() { return glm::vec4(m_Position, 1.0f); }
    glm::vec4 getLightColorAndIntensity() { return glm::vec4(m_Color, m_Intensity); }

protected:

    void initMeshRender();

    glm::vec3 m_Position;
    glm::vec3 m_Color;
    float m_Intensity;

    std::shared_ptr<ShapeRenderer> m_ShapeRenderer;
};