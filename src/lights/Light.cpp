#include "lights/Light.h"

Light::Light(const glm::vec3 &position, const glm::vec3 &color, const bool &castShadows)
    : m_Position(position), m_Color(color), m_CastShadows(castShadows)
{ }

glm::vec3& Light::GetLightPosition()
{
    return m_Position;
}

glm::vec3& Light::GetLightColor()
{
    return m_Color;
}

bool& Light::IsCastShadow()
{
    return m_CastShadows;
}

void Light::SetCastShadow(const bool &bCastShadow)
{
    m_CastShadows = bCastShadow;
}
