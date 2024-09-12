#ifndef SCREEN_SPACE_AMBIENT_OCCLUSION_H
#define SCREEN_SPACE_AMBIENT_OCCLUSION_H

#include "ptr.h"
#include "base/Texture2D.h"
#include "renderer/RenderTarget.h"
#include "base/Material.h"

#include "renderer/GLStateCache.h"

class ScreenSpaceAmbientOcclusion
{
    SHARED_PTR(ScreenSpaceAmbientOcclusion)
public:
    ScreenSpaceAmbientOcclusion();
    ~ScreenSpaceAmbientOcclusion() = default;
    
    void SetRenderSize(const size_t &width, const size_t &height);
    void CopyDepth(const RenderTarget::Ptr source);
    void Render(const RenderTarget::Ptr source, const GLStateCache::Ptr glStateCache);

    Texture2D::Ptr GetSSAO();
    Texture2D::Ptr GetFinalSSAO();
    
private:
    
    Material::Ptr m_SSAOMat;
    Material::Ptr m_BilateralBlurMat;
    Material::Ptr m_FinalBilateralBlurMat;
    
    RenderTarget::Ptr m_SSAORenderTarget;
    RenderTarget::Ptr m_FinalSSAO;
};

#endif
