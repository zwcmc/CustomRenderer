#pragma once

#include <glm/glm.hpp>

#include "ptr.h"
#include "lights/Light.h"

#include "renderer/RenderTarget.h"

class DirectionalLight : public Light
{
    SHARED_PTR(DirectionalLight)
public:
    DirectionalLight(const glm::vec3 &direction, const glm::vec3 &color, const bool &castShadows = false);
    ~DirectionalLight() = default;
    
    virtual RenderTarget::Ptr GetShadowMapRT() override;
    virtual Texture2D::Ptr GetEmptyShadowMapTexture() override;
    
    glm::u32vec2& GetShadowMapSize();
    
private:
    RenderTarget::Ptr m_ShadowMapRT;
    Texture2D::Ptr m_EmptyShadowMapTexture;

    glm::u32vec2 m_ShadowMapSize;
};
