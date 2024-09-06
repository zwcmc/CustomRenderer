#include "renderer/Blitter.h"

#include <assert.h>

GLuint Blitter::BlitVAO = 0;
Material::Ptr Blitter::DefaultBlitMat = nullptr;
Material::Ptr Blitter::BlitDepthMat = nullptr;

void Blitter::BlitToTarget(const Texture2D::Ptr sourceTex, const RenderTarget::Ptr target, Material::Ptr blitMat)
{
    assert(sourceTex != nullptr || target != nullptr);
    
    target->BindTarget(true, true);

    if (blitMat == nullptr)
    {
        blitMat = DefaultBlitMat;
    }

    sourceTex->SetTextureName("uSourceTex");
    blitMat->AddOrSetTexture(sourceTex);

    blitMat->Use();

    DrawFullScreenTriangle();
    
    target->UnbindTarget();
}

void Blitter::BlitToTarget(const RenderTarget::Ptr source, const RenderTarget::Ptr target, Material::Ptr blitMat)
{
    assert(source != nullptr || target != nullptr);

    BlitToTarget(source->GetColorTexture(0), target, blitMat);
}

void Blitter::BlitToCameraTarget(const RenderTarget::Ptr source, const Camera::Ptr targetCamera, Material::Ptr blitMat)
{
    assert(source != nullptr);

    targetCamera->BindCameraTarget();

    if (blitMat == nullptr)
    {
        blitMat = DefaultBlitMat;
    }

    // Setup source texture
    source->GetColorTexture(0)->SetTextureName("uSourceTex");
    blitMat->AddOrSetTexture(source->GetColorTexture(0));

    blitMat->Use();

    DrawFullScreenTriangle();
}

void Blitter::BlitToCameraTarget(const Texture2D::Ptr sourceTex, const Camera::Ptr targetCamera, Material::Ptr blitMat)
{
    assert(sourceTex != nullptr);
    
    targetCamera->BindCameraTarget();

    if (blitMat == nullptr)
    {
        blitMat = DefaultBlitMat;
    }

    // Setup source texture
    sourceTex->SetTextureName("uSourceTex");
    blitMat->AddOrSetTexture(sourceTex);

    blitMat->Use();

    DrawFullScreenTriangle();
}

void Blitter::RenderToTarget(const RenderTarget::Ptr target, Material::Ptr renderMat)
{
    assert(target != nullptr || renderMat != nullptr);

    target->BindTarget(true, true);

    renderMat->Use();
    DrawFullScreenTriangle();

    target->UnbindTarget();
}

void Blitter::CopyDepth(const RenderTarget::Ptr source, const RenderTarget::Ptr target)
{
    target->BindTarget(false, true);

    BlitDepthMat->AddOrSetTexture(source->GetDepthTexture());
    BlitDepthMat->Use();

    DrawFullScreenTriangle();
}

void Blitter::BlitDepth(const RenderTarget::Ptr source, const RenderTarget::Ptr target)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, source->GetFrameBufferID());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target->GetFrameBufferID());

    glBlitFramebuffer(0, 0, source->GetSize().x, source->GetSize().y, 0, 0, target->GetSize().x, target->GetSize().y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Blitter::Init()
{
    if (BlitVAO == 0)
    {
        glGenVertexArrays(1, &BlitVAO);
    }

    if (DefaultBlitMat == nullptr)
    {
        DefaultBlitMat = Material::New("Blit", "utils/FullScreenTriangle.vs", "utils/BlitColor.fs");
    }
    
    if (BlitDepthMat == nullptr)
    {
        BlitDepthMat = Material::New("Blit Depth", "utils/FullScreenTriangle.vs", "utils/CopyDepth.fs");
    }
}

void Blitter::Cleanup()
{
    glDeleteVertexArrays(1, &BlitVAO);
}

void Blitter::DrawFullScreenTriangle()
{
    glBindVertexArray(BlitVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}
