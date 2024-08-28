#include "lights/DirectionalLight.h"

DirectionalLight::DirectionalLight(const glm::vec3 &direction, const glm::vec3 &color, const bool &castShadows)
    : Light(direction, color, castShadows)
{
    if (castShadows)
    {
        m_ShadowMapRT = RenderTarget::New(2048, 2048, GL_FLOAT, 0, false, true);
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
