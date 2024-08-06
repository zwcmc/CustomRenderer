#include "lights/DirectionalLight.h"

DirectionalLight::DirectionalLight(const glm::vec3 & lightDirection, const glm::vec3 &color)
    : BaseLight(lightDirection, color)
{ }