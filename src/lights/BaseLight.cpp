#include "lights/BaseLight.h"

BaseLight::BaseLight()
    : m_Position(glm::vec3(0.0f)), m_Intensity(1.0f), m_Color(glm::vec3(1.0f))
{ }

BaseLight::BaseLight(const glm::vec3 &position, const glm::vec3 &color, const float &intensity)
{
    m_Position = position;
    m_Color = color;
    m_Intensity = intensity;
}