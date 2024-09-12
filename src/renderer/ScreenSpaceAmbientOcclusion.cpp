#include "renderer/ScreenSpaceAmbientOcclusion.h"

#include "renderer/Blitter.h"

ScreenSpaceAmbientOcclusion::ScreenSpaceAmbientOcclusion()
{
    m_SSAOMat = Material::New("SSAO", "ssao/SSAO.vs", "ssao/SSAO.fs");
    m_BilateralBlurMat = Material::New("Bilateral Blur", "ssao/SSAO.vs", "ssao/BilateralBlur.fs");
    m_FinalBilateralBlurMat = Material::New("Final Bilateral Blur", "ssao/SSAO.vs", "ssao/FinalBilateralBlur.fs");
    
    m_SSAORenderTarget = RenderTarget::New(1, 1, GL_HALF_FLOAT, 1, true);
    m_FinalSSAO = RenderTarget::New(1, 1, GL_HALF_FLOAT, 1);
}

void ScreenSpaceAmbientOcclusion::SetRenderSize(const size_t &width, const size_t &height)
{
    m_SSAORenderTarget->SetSize(width, height);
    m_FinalSSAO->SetSize(width, height);
}

void ScreenSpaceAmbientOcclusion::CopyDepth(const RenderTarget::Ptr source)
{
    Blitter::CopyDepth(source, m_SSAORenderTarget);
}

void ScreenSpaceAmbientOcclusion::Render(const RenderTarget::Ptr source, const GLStateCache::Ptr glStateCache)
{
    // Copy depth to ssao render target for depth testing to skip pixels at max 1.0 (i.e. the skybox)
    CopyDepth(source);

    glStateCache->SetDepthFunc(GL_NOTEQUAL);

    m_SSAOMat->AddOrSetTexture("uDepthTexture", source->GetDepthTexture());
    Blitter::RenderToTarget(m_SSAORenderTarget, m_SSAOMat, true, false);

    glStateCache->SetDepthTest(false);
    
    // bilateral blur horizontal
    glm::u32vec2 size = m_SSAORenderTarget->GetSize();
    glm::vec4 offset = glm::vec4(1.0f / size.x, 0.0f, 0.0f, 0.0f);
    m_BilateralBlurMat->AddOrSetVector("uOffset", offset);
    Blitter::BlitCameraTexture(m_SSAORenderTarget, m_FinalSSAO, m_BilateralBlurMat);
    
    // bilateral blur vertical
    offset = glm::vec4(0.0f, 1.0f / size.y, 0.0f, 0.0f);
    m_BilateralBlurMat->AddOrSetVector("uOffset", offset);
    Blitter::BlitCameraTexture(m_FinalSSAO, m_SSAORenderTarget, m_BilateralBlurMat);
    
    // final bilateral blur
    Blitter::BlitCameraTexture(m_SSAORenderTarget, m_FinalSSAO, m_FinalBilateralBlurMat);
}

Texture2D::Ptr ScreenSpaceAmbientOcclusion::GetSSAO()
{
    return m_SSAORenderTarget->GetColorTexture(0);
}

Texture2D::Ptr ScreenSpaceAmbientOcclusion::GetFinalSSAO()
{
    return m_FinalSSAO->GetColorTexture(0);
}
