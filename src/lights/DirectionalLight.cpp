#include "lights/DirectionalLight.h"

DirectionalLight::DirectionalLight(const glm::vec3 &direction, const glm::vec3 &color, const bool &castShadows)
    : Light(direction, color, castShadows, glm::u32vec2(2048))
{
    if (castShadows)
    {
        m_ShadowMapRT = RenderTarget::New(m_ShadowMapSize.x, m_ShadowMapSize.y, GL_FLOAT, 0, false, true);
    }
    else
    {
        m_EmptyShadowMapTexture = Texture2D::New("uShadowMap");
        m_EmptyShadowMapTexture->InitShadowMap(glm::u32vec2(1));
    }
}

RenderTarget::Ptr DirectionalLight::GetShadowMapRT()
{
    return m_ShadowMapRT;
}

Texture2D::Ptr DirectionalLight::GetEmptyShadowMapTexture()
{
    return m_EmptyShadowMapTexture;
}
