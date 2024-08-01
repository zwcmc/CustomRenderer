#pragma once

#include "ptr.h"
#include "renderer/Camera.h"
#include "lights/BaseLight.h"

class ModelRenderer
{
SHARED_PTR(ModelRenderer)
public:
    ModelRenderer() = default;
    ~ModelRenderer() = default;

    virtual void translate(const glm::vec3 &p) { }
    virtual void scale(const glm::vec3 &s) { }
    virtual void rotate(const float &radians, const glm::vec3 &axis) { }

    virtual void draw(Camera::Ptr camera) { }
    virtual void draw(Camera::Ptr camera, BaseLight::Ptr light) {}
};