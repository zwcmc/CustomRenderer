#include "renderer/PostProcessing.h"

#include <glm/glm.hpp>
#include <cmath>

#include "renderer/Blitter.h"

#include "utility/StatusRecorder.h"

PostProcessing::PostProcessing()
{
    m_BloomDownsample2xMat = Material::New("Bloom Prefilter", "utils/FullScreenTriangle.vs", "post_processing/bloom/BloomDownsample2x.fs");

    m_BloomBlurHorizontal = Material::New("Bloom Blur Horizontal", "utils/FullScreenTriangle.vs", "post_processing/bloom/BloomBlurHorizontal.fs");
    m_BloomBlurVertical = Material::New("Bloom Blur Vertical", "utils/FullScreenTriangle.vs", "post_processing/bloom/BloomBlurVertical.fs");
    m_BloomUpsample = Material::New("Bloom Upsample", "utils/FullScreenTriangle.vs", "post_processing/bloom/BloomUpsample.fs");

    m_CombinePostMat = Material::New("Combine Post", "utils/FullScreenTriangle.vs", "post_processing/CombinePost.fs");

    m_FinalPostMat = Material::New("Final Post", "utils/FullScreenTriangle.vs", "post_processing/FinalPost.fs");
}

PostProcessing::~PostProcessing()
{
    m_BloomMipUp.clear();
    m_BloomMipDown.clear();
}

void PostProcessing::Render(const RenderTarget::Ptr source, const Camera::Ptr targetCamera)
{
    bool bloomActive = StatusRecorder::Bloom;
    if (bloomActive)
    {
        // Bloom
        RenderTarget::Ptr bloom = RenderTarget::New(1, 1, GL_HALF_FLOAT, 1);
        Bloom(source, bloom);

        m_CombinePostMat->AddOrSetFloat("uBloomSet", 1.0f);
        m_CombinePostMat->AddOrSetTexture("uBloomTex", bloom->GetColorTexture(0));
    }
    else
    {
        m_CombinePostMat->AddOrSetFloat("uBloomSet", -1.0f);
    }
    
    bool fxaaActive = StatusRecorder::FXAA;
    if (fxaaActive)
    {
        // Combine post-processing
        m_CombinePostMat->AddOrSetFloat("uBloomIntensity", StatusRecorder::BloomIntensity);
        m_CombinePostMat->AddOrSetFloat("uBlitToCamera", -1.0f);
        m_CombinePostMat->AddOrSetFloat("uToneMappingSet", StatusRecorder::ToneMapping ? 1.0 : -1.0);
        RenderTarget::Ptr tempRT = RenderTarget::New(source->GetSize(), GL_HALF_FLOAT, 1);
        Blitter::BlitCameraTexture(source, tempRT, m_CombinePostMat);

        // Blit to camera with FXAA
        m_FinalPostMat->AddOrSetFloat("uFXAASet", 1.0f);
        m_FinalPostMat->AddOrSetFloat("uToneMappingSet", StatusRecorder::ToneMapping ? 1.0 : -1.0);
        Blitter::BlitCamera(tempRT, targetCamera, m_FinalPostMat);
    }
    else
    {
        // Combine post-processing
        m_CombinePostMat->AddOrSetFloat("uBloomIntensity", StatusRecorder::BloomIntensity);
        m_CombinePostMat->AddOrSetFloat("uBlitToCamera", 1.0f);
        m_CombinePostMat->AddOrSetFloat("uToneMappingSet", StatusRecorder::ToneMapping ? 1.0 : -1.0);
        Blitter::BlitCamera(source, targetCamera, m_CombinePostMat);
    }
}

void PostProcessing::Bloom(const RenderTarget::Ptr source, RenderTarget::Ptr &bloomRT)
{
    glm::u32vec2 size = source->GetSize();
    
    int tw = size.x >> 1;
    int th = size.y >> 1;

    int maxSize = glm::max(tw, th);
    int mipCount = glm::floor(std::log2(maxSize) - 1);

    // Resize final bloom texture
    bloomRT->SetSize(tw, th);
    
    if (m_BloomMipUp.size() < mipCount)
    {
        m_BloomMipUp.resize(mipCount);
    }
    if (m_BloomMipDown.size() < mipCount)
    {
        m_BloomMipDown.resize(mipCount);
    }
    
    for (size_t i = 0; i < mipCount; ++i)
    {
        ReAllocateOrReSetSizeBloomRT(m_BloomMipUp[i], tw, th);
        ReAllocateOrReSetSizeBloomRT(m_BloomMipDown[i], tw, th);
        
        tw = glm::max(1, tw >> 1);
        th = glm::max(1, th >> 1);
    }

    // 2x downsampling
    Blitter::BlitCameraTexture(source, m_BloomMipDown[0], m_BloomDownsample2xMat);
    
    // Downsample - gaussian pyramid
    RenderTarget::Ptr lastDown = m_BloomMipDown[0];

    for (size_t i = 1; i < mipCount; ++i)
    {
        // 2x downsampling + 9-tap gaussian
        Blitter::BlitCameraTexture(lastDown, m_BloomMipUp[i], m_BloomBlurHorizontal);
        // 5-tap filter + bilinear filtering
        Blitter::BlitCameraTexture(m_BloomMipUp[i], m_BloomMipDown[i], m_BloomBlurVertical);

        lastDown = m_BloomMipDown[i];
    }

    // Upsample
//    float scatter = glm::mix(0.0f, 1.0f, StatusRecorder::BloomScatter);
    m_BloomUpsample->AddOrSetFloat("uScatter", StatusRecorder::BloomScatter);
    for (int i = mipCount - 2; i >= 0; --i)
    {
        RenderTarget::Ptr lowMip = (i == mipCount - 2) ? m_BloomMipDown[i + 1] : m_BloomMipUp[i + 1];
        RenderTarget::Ptr highMip = m_BloomMipDown[i];
        RenderTarget::Ptr dst = m_BloomMipUp[i];

        m_BloomUpsample->AddOrSetTexture("uLowSourceTex", lowMip->GetColorTexture(0));
        Blitter::BlitCameraTexture(highMip, dst, m_BloomUpsample);
    }
    
    Blitter::BlitCameraTexture(m_BloomMipUp[0], bloomRT);
}

void PostProcessing::ReAllocateOrReSetSizeBloomRT(RenderTarget::Ptr &rt, const unsigned int &width, const unsigned int &height)
{
    if (rt == nullptr)
    {
        rt = RenderTarget::New(width, height, GL_HALF_FLOAT, 1);
    }
    else
    {
        if (rt->GetSize().x != width || rt->GetSize().y != height)
        {
            rt->SetSize(width, height);
        }
    }
}
