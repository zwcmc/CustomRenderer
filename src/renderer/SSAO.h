#ifndef SSAO_H
#define SSAO_H

#include "ptr.h"
#include "base/Texture2D.h"
#include "renderer/RenderTarget.h"
#include "base/Material.h"

class SSAO
{
    SHARED_PTR(SSAO)
public:
    SSAO();
    ~SSAO() = default;
    
    void SetRenderTargetSize(const size_t &width, const size_t &height);
    
    void CopyDepth(const RenderTarget::Ptr source);
    void Render(const RenderTarget::Ptr source);
    
    Texture2D::Ptr GetSAO();
    
private:
    
    Material::Ptr m_SAO;
    RenderTarget::Ptr m_SSAOBuffer;
};

#endif
