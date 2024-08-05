#pragma once

#include <glm/glm.hpp>

#include "ptr.h"
#include "lights/BaseLight.h"

class DirectionalLight : public BaseLight
{
    SHARED_PTR(DirectionalLight)
public:
    DirectionalLight(const glm::vec3 &lightDirection, const glm::vec3 &color, const float &intensity);
    ~DirectionalLight() = default;
};