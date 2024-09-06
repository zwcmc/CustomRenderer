#include "renderer/Blitter.h"

#include <assert.h>

GLuint Blitter::BlitVAO = 0;
Material::Ptr Blitter::DefaultBlitMat = nullptr;
Material::Ptr Blitter::BlitDepthMat = nullptr;

void Blitter::BlitCameraTexture(const Texture2D::Ptr sourceTex, const RenderTarget::Ptr destination, Material::Ptr material)
{
    assert(sourceTex != nullptr || destination != nullptr);
    
    destination->BindTarget(true, true);

    if (material == nullptr)
    {
        material = DefaultBlitMat;
    }

    sourceTex->SetTextureName("uSourceTex");
    material->AddOrSetTexture(sourceTex);

    material->Use();

    DrawFullScreenTriangle();
}

void Blitter::BlitCameraTexture(const RenderTarget::Ptr source, const RenderTarget::Ptr destination, Material::Ptr material)
{
    BlitCameraTexture(source->GetColorTexture(0), destination, material);
}

void Blitter::BlitCamera(const Texture2D::Ptr sourceTex, const Camera::Ptr camera, Material::Ptr material)
{
    assert(sourceTex != nullptr || camera != nullptr);
    
    camera->BindCameraTarget(true, true);
    
    if (material == nullptr)
    {
        material = DefaultBlitMat;
    }
    
    sourceTex->SetTextureName("uSourceTex");
    material->AddOrSetTexture(sourceTex);

    material->Use();

    DrawFullScreenTriangle();
}

void Blitter::BlitCamera(const RenderTarget::Ptr source, const Camera::Ptr camera, Material::Ptr material)
{
    BlitCamera(source->GetColorTexture(0), camera, material);
}

void Blitter::RenderToTarget(const RenderTarget::Ptr target, Material::Ptr material)
{
    assert(target != nullptr || material != nullptr);

    target->BindTarget(true, true);

    material->Use();
    DrawFullScreenTriangle();

    target->UnbindTarget();
}

void Blitter::CopyDepth(const RenderTarget::Ptr source, const RenderTarget::Ptr destination)
{
    destination->BindTarget(false, true);

    BlitDepthMat->AddOrSetTexture(source->GetDepthTexture());
    BlitDepthMat->Use();

    DrawFullScreenTriangle();
    
    destination->UnbindTarget();
}

void Blitter::BlitDepth(const RenderTarget::Ptr source, const RenderTarget::Ptr destination)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, source->GetFrameBufferID());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destination->GetFrameBufferID());

    glBlitFramebuffer(0, 0, source->GetSize().x, source->GetSize().y, 0, 0, destination->GetSize().x, destination->GetSize().y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    
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
