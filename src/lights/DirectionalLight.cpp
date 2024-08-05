#include "lights/DirectionalLight.h"

DirectionalLight::DirectionalLight(const glm::vec3 & lightDirection, const glm::vec3 &color, const float &intensity)
    : BaseLight(lightDirection, color, intensity)
{ }