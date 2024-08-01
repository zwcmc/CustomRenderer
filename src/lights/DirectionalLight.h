#pragma once

#include "ptr.h"
#include <glm/glm.hpp>
#include "lights/BaseLight.h"

class DirectionalLight : public BaseLight
{
SHARED_PTR(DirectionalLight)
public:
    DirectionalLight(const glm::vec3 &position, const glm::vec3 &color, const float &intensity);
    ~DirectionalLight() = default;
};