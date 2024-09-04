#include "renderer/PostProcessing.h"

#include <glm/glm.hpp>
#include <cmath>

#include "renderer/Blitter.h"

#include "utility/StatsRecorder.h"

PostProcessing::PostProcessing()
{
    m_BloomDownsample2xMat = Material::New("Bloom Prefilter", "post_processing/Blit.vert", "post_processing/bloom/BloomDownsample2x.frag");

    m_BloomBlurHorizontal = Material::New("Bloom Blur Horizontal", "post_processing/Blit.vert", "post_processing/bloom/BloomBlurHorizontal.frag");
    m_BloomBlurVertical = Material::New("Bloom Blur Vertical", "post_processing/Blit.vert", "post_processing/bloom/BloomBlurVertical.frag");
    m_BloomUpsample = Material::New("Bloom Upsample", "post_processing/Blit.vert", "post_processing/bloom/BloomUpsample.frag");
    
    m_CombinePostMat = Material::New("Combine Post", "post_processing/Blit.vert", "post_processing/CombinePost.frag");
    
    m_FinalPostMat = Material::New("Final Post", "post_processing/Blit.vert", "post_processing/FinalPost.frag");
}

PostProcessing::~PostProcessing()
{
    m_BloomMipUp.clear();
    m_BloomMipDown.clear();
}

void PostProcessing::Render(const RenderTarget::Ptr source, const Camera::Ptr targetCamera)
{
    bool bloomActive = StatsRecorder::BloomOn;
    if (bloomActive)
    {
        // Bloom
        RenderTarget::Ptr bloom = RenderTarget::New(1, 1, GL_HALF_FLOAT, 1);
        SetupBloom(source, bloom);
        
        m_CombinePostMat->AddOrSetFloat("uBloomSet", 1.0f);
        m_CombinePostMat->AddOrSetTexture("uBloomTex", bloom->GetColorTexture(0));
    }
    else
    {
        m_CombinePostMat->AddOrSetFloat("uBloomSet", -1.0f);
    }
    
    bool fxaaActive = StatsRecorder::FXAAOn;
    if (fxaaActive)
    {
        // Combine post-processing
        m_CombinePostMat->AddOrSetFloat("uBloomIntensity", StatsRecorder::BloomIntensity);
        m_CombinePostMat->AddOrSetFloat("uBlitToCamera", -1.0f);
        m_CombinePostMat->AddOrSetFloat("uToneMappingSet", StatsRecorder::ToneMappingOn ? 1.0 : -1.0);
        RenderTarget::Ptr tempRT = RenderTarget::New(source->GetSize(), GL_HALF_FLOAT, 1);
        Blitter::BlitToTarget(source, tempRT, m_CombinePostMat);

        // Blit to camera with FXAA
        m_FinalPostMat->AddOrSetFloat("uFXAASet", 1.0f);
        m_FinalPostMat->AddOrSetFloat("uToneMappingSet", StatsRecorder::ToneMappingOn ? 1.0 : -1.0);
        Blitter::BlitToCameraTarget(tempRT, targetCamera, m_FinalPostMat);
    }
    else
    {
        // Combine post-processing
        m_CombinePostMat->AddOrSetFloat("uBloomIntensity", StatsRecorder::BloomIntensity);
        m_CombinePostMat->AddOrSetFloat("uBlitToCamera", 1.0f);
        m_CombinePostMat->AddOrSetFloat("uToneMappingSet", StatsRecorder::ToneMappingOn ? 1.0 : -1.0);
        Blitter::BlitToCameraTarget(source, targetCamera, m_CombinePostMat);
    }
}

void PostProcessing::SetupBloom(const RenderTarget::Ptr source, RenderTarget::Ptr &bloomRT)
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
    Blitter::BlitToTarget(source, m_BloomMipDown[0], m_BloomDownsample2xMat);
    
    // Downsample - gaussian pyramid
    RenderTarget::Ptr lastDown = m_BloomMipDown[0];

    for (size_t i = 1; i < mipCount; ++i)
    {
        // 2x downsampling + 9-tap gaussian
        Blitter::BlitToTarget(lastDown, m_BloomMipUp[i], m_BloomBlurHorizontal);
        // 5-tap filter + bilinear filtering
        Blitter::BlitToTarget(m_BloomMipUp[i], m_BloomMipDown[i], m_BloomBlurVertical);

        lastDown = m_BloomMipDown[i];
    }

    // Upsample
//    float scatter = glm::mix(0.0f, 1.0f, StatsRecorder::BloomScatter);
    m_BloomUpsample->AddOrSetFloat("uScatter", StatsRecorder::BloomScatter);
    for (int i = mipCount - 2; i >= 0; --i)
    {
        RenderTarget::Ptr lowMip = (i == mipCount - 2) ? m_BloomMipDown[i + 1] : m_BloomMipUp[i + 1];
        RenderTarget::Ptr highMip = m_BloomMipDown[i];
        RenderTarget::Ptr dst = m_BloomMipUp[i];

        m_BloomUpsample->AddOrSetTexture("uLowSourceTex", lowMip->GetColorTexture(0));
        Blitter::BlitToTarget(highMip, dst, m_BloomUpsample);
    }
    
    Blitter::BlitToTarget(m_BloomMipUp[0], bloomRT);
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
