#pragma once

#include <glm/glm.hpp>
#include "ptr.h"
#include "renderer/RenderTarget.h"

class Light
{
    SHARED_PTR(Light)
public:
    Light(const glm::vec3 &position, const glm::vec3 &color, const bool &castShadows, const glm::u32vec2 &shadowMapSize = glm::u32vec2(1));
    ~Light() = default;

    glm::vec3& GetLightPosition();
    glm::vec3& GetLightColor();
    
    bool& IsCastShadow();
    void SetCastShadow(const bool &bCastShadow);

    virtual RenderTarget::Ptr GetShadowMapRT() = 0;
    virtual Texture2D::Ptr GetEmptyShadowMapTexture() = 0;
    glm::u32vec2 &GetShadowMapSize();

protected:
    glm::u32vec2 m_ShadowMapSize;

private:
    glm::vec3 m_Position;
    glm::vec3 m_Color;

    bool m_CastShadows;
};
