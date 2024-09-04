#include "renderer/Blitter.h"

#include <assert.h>

GLuint Blitter::BlitVAO = 0;
Material::Ptr Blitter::Mat = nullptr;

void Blitter::BlitToTarget(const Texture2D::Ptr sourceTex, const RenderTarget::Ptr target, Material::Ptr blitMat)
{
    assert(sourceTex != nullptr || target != nullptr);

    target->Bind();

    if (blitMat == nullptr)
        blitMat = Mat;

    sourceTex->SetTextureName("uSourceTex");
    blitMat->AddOrSetTexture(sourceTex);

    blitMat->Use();

    glBindVertexArray(BlitVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    
    target->Unbind();
}

void Blitter::BlitToTarget(const RenderTarget::Ptr source, const RenderTarget::Ptr target, Material::Ptr blitMat)
{
    assert(source != nullptr || target != nullptr);

    if (blitMat == nullptr)
    {
        blitMat = Mat;
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
        blitMat = Mat;

    // Setup source texture
    source->GetColorTexture(0)->SetTextureName("uSourceTex");
    blitMat->AddOrSetTexture(source->GetColorTexture(0));

    blitMat->Use();

    glBindVertexArray(BlitVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void Blitter::BlitToCameraTarget(const Texture2D::Ptr sourceTex, const Camera::Ptr targetCamera, Material::Ptr blitMat)
{
    assert(sourceTex != nullptr);
    
    targetCamera->BindCameraTarget();

    if (blitMat == nullptr)
        blitMat = Mat;

    // Setup source texture
    sourceTex->SetTextureName("uSourceTex");
    blitMat->AddOrSetTexture(sourceTex);

    blitMat->Use();

    glBindVertexArray(BlitVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void Blitter::RenderToTarget(const RenderTarget::Ptr target, Material::Ptr blitMat)
{
    assert(target != nullptr);

    target->Bind();

    if (blitMat == nullptr)
        blitMat = Mat;

    blitMat->Use();

    glBindVertexArray(BlitVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    
    target->Unbind();
}

void Blitter::Init()
{
    if (BlitVAO == 0)
    {
        glGenVertexArrays(1, &BlitVAO);
    }

    if (Mat == nullptr)
    {
        Mat = Material::New("Blit", "post_processing/Blit.vert", "post_processing/Blit.frag");
    }
}

void Blitter::Cleanup()
{
    glDeleteVertexArrays(1, &BlitVAO);
}
