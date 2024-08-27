#pragma once

#include <glm/glm.hpp>
#include "ptr.h"
#include "renderer/RenderTarget.h"

class Light
{
    SHARED_PTR(Light)
public:
    Light(const glm::vec3 &position, const glm::vec3 &color, const bool &castShadows);
    ~Light() = default;

    glm::vec3& GetLightPosition();
    glm::vec3& GetLightColor();
    
    bool& IsCastShadow();
    void SetCastShadow(const bool &bCastShadow);
    
    virtual RenderTarget::Ptr GetShadowMapRT() { return nullptr; }

private:
    glm::vec3 m_Position;
    glm::vec3 m_Color;
    
    bool m_CastShadows;
};
