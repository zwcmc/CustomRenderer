#pragma once

#include "ptr.h"
#include "base/Material.h"
#include "renderer/RenderTarget.h"
#include "cameras/Camera.h"

class PostProcessing
{
    SHARED_PTR(PostProcessing)
public:
    PostProcessing();
    ~PostProcessing() = default;
    
    void Render(const RenderTarget::Ptr source, const Camera::Ptr targetCamera);

private:

    Material::Ptr m_FXAAMat;
};
