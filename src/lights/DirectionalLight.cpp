#include "lights/DirectionalLight.h"

DirectionalLight::DirectionalLight(const glm::vec3 &position, const glm::vec3 &color, const float &intensity)
    : BaseLight(position, color, intensity)
{ }