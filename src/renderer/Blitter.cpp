#include "renderer/Blitter.h"

#include <assert.h>

GLuint Blitter::BlitVAO = 0;
Material::Ptr Blitter::DefaultBlitMat = nullptr;

void Blitter::BlitToTarget(const Texture2D::Ptr sourceTex, const RenderTarget::Ptr target, Material::Ptr blitMat)
{
    assert(sourceTex != nullptr || target != nullptr);

    target->Bind();

    if (blitMat == nullptr)
    {
        blitMat = DefaultBlitMat;
    }

    sourceTex->SetTextureName("uSourceTex");
    blitMat->AddOrSetTexture(sourceTex);

    blitMat->Use();

    DrawFullScreenTriangle();
    
    target->Unbind();
}

void Blitter::BlitToTarget(const RenderTarget::Ptr source, const RenderTarget::Ptr target, Material::Ptr blitMat)
{
    assert(source != nullptr || target != nullptr);

    if (blitMat == nullptr)
    {
        blitMat = DefaultBlitMat;
    }
    
    Texture2D::Ptr sourceTex = source->GetColorTexture(0);
    if (sourceTex)
    {
        BlitToTarget(sourceTex, target, blitMat);
    }
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

void Blitter::RenderToTarget(const RenderTarget::Ptr target, Material::Ptr blitMat)
{
    assert(target != nullptr);

    target->Bind();

    FullScreenRender(blitMat);
    
    target->Unbind();
}

void Blitter::FullScreenRender(Material::Ptr blitMat)
{
    if (blitMat == nullptr)
    {
        blitMat = DefaultBlitMat;
    }

    blitMat->Use();

    DrawFullScreenTriangle();
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
