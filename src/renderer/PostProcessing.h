#pragma once

#include <vector>

#include "ptr.h"
#include "base/Material.h"
#include "renderer/RenderTarget.h"
#include "cameras/Camera.h"

class PostProcessing
{
    SHARED_PTR(PostProcessing)
public:
    PostProcessing();
    ~PostProcessing();
    
    void Render(const RenderTarget::Ptr source, const Camera::Ptr targetCamera);

private:

    void Bloom(const RenderTarget::Ptr source, RenderTarget::Ptr &bloomRT);

    void ReAllocateOrReSetSizeBloomRT(RenderTarget::Ptr &rt, const unsigned int &width, const unsigned int &height);

    // Bloom
    std::vector<RenderTarget::Ptr> m_BloomMipUp;
    std::vector<RenderTarget::Ptr> m_BloomMipDown;

    Material::Ptr m_BloomDownsample2xMat;
    Material::Ptr m_BloomBlurHorizontal;
    Material::Ptr m_BloomBlurVertical;
    Material::Ptr m_BloomUpsample;
    
    // Combine post-processing
    Material::Ptr m_CombinePostMat;
    
    // Final post
    Material::Ptr m_FinalPostMat;
};
