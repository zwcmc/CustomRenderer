#pragma once

#include "ptr.h"
#include "renderer/Camera.h"

class ModelRenderer
{
SHARED_PTR(ModelRenderer)
public:
    ModelRenderer() = default;
    ~ModelRenderer() = default;

    virtual void draw(Camera::Ptr camera) { }
};