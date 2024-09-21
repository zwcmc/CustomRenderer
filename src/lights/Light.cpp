#include "lights/Light.h"

Light::Light(const glm::vec3 &position, const glm::vec3 &color, const bool &castShadows, const glm::u32vec2 &shadowMapSize)
    : m_Position(position), m_Color(color), m_CastShadows(castShadows), m_ShadowMapSize(shadowMapSize)
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

glm::u32vec2& Light::GetShadowMapSize()
{
    return m_ShadowMapSize;
}
