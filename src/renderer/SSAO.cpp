#include "renderer/SSAO.h"

#include "renderer/Blitter.h"

#include <random>

SSAO::SSAO()
{
    m_SAO = Material::New("SSAO Buffer", "ssao/SAOFullScreenTriangle.vs", "ssao/SAOBuffer.fs");
    m_SSAOBuffer = RenderTarget::New(1, 1, GL_HALF_FLOAT, 1, true);
}

void SSAO::SetRenderTargetSize(const size_t &width, const size_t &height)
{
    m_SSAOBuffer->SetSize(width, height);
}

void SSAO::CopyDepth(const RenderTarget::Ptr source)
{
    Blitter::CopyDepth(source, m_SSAOBuffer);
}

void SSAO::Render(const RenderTarget::Ptr source)
{
    m_SAO->AddOrSetTexture("uNormalRoughness", source->GetColorTexture(1));
    m_SAO->AddOrSetTexture("uPositionOcclusion", source->GetColorTexture(2));
    
    int SSAOKernelSize = 64;
    std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
    std::default_random_engine generator;
    std::vector<glm::vec4> ssaoKernel;
    for (int i = 0; i < SSAOKernelSize; ++i)
    {
        glm::vec4 sample(
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator),
            0.0f
        );
        sample = glm::normalize(sample);
        sample = sample * randomFloats(generator);
        float scale = (float)i / (float)SSAOKernelSize;
        scale = glm::mix(0.1f, 1.0f, scale * scale);
        sample = sample * scale;
        ssaoKernel.push_back(sample);
    }
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f);
        ssaoNoise.push_back(noise);
    }
    Texture2D::Ptr m_SSAONoise = Texture2D::New("uNoise");
    m_SSAONoise->InitTexture2D(glm::u32vec2(4, 4), GL_RGBA16F, GL_RGB, GL_HALF_FLOAT, &ssaoNoise[0]);
    
    m_SAO->AddOrSetTexture("uNoise", m_SSAONoise);
    
    m_SAO->GetShader()->Use();
    m_SAO->GetShader()->SetUniformVectorArray("uKernels", ssaoKernel.size(), ssaoKernel);
    
    Blitter::RenderToTarget(m_SSAOBuffer, m_SAO, true, false);
}


Texture2D::Ptr SSAO::GetSAO()
{
    return m_SSAOBuffer->GetColorTexture(0);
}
