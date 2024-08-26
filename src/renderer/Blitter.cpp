#include "renderer/Blitter.h"

#include <assert.h>

GLuint Blitter::BlitVAO = 0;
Material::Ptr Blitter::Mat = nullptr;

void Blitter::BlitToTarget(Texture2D::Ptr source, RenderTarget::Ptr target, Material::Ptr blitMat)
{
    assert(source != nullptr || target != nullptr);

    target->Bind();

    if (blitMat == nullptr)
        blitMat = Mat;

    source->SetTextureName("uSource");
    blitMat->AddOrSetTexture(source);

    blitMat->Use();

    glBindVertexArray(BlitVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void Blitter::BlitToCamera(Texture2D::Ptr source, glm::u32vec2 size, Material::Ptr blitMat)
{
    assert(source != nullptr);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glViewport(0, 0, size.x, size.y);

    if (blitMat == nullptr)
        blitMat = Mat;

    source->SetTextureName("uSource");
    blitMat->AddOrSetTexture(source);

    blitMat->Use();

    glBindVertexArray(BlitVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void Blitter::RenderToTarget(RenderTarget::Ptr target, Material::Ptr blitMat)
{
    assert(target != nullptr);

    target->Bind();

    if (blitMat == nullptr)
        blitMat = Mat;

    blitMat->Use();

    glBindVertexArray(BlitVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void Blitter::Init()
{
    if (BlitVAO == 0)
        glGenVertexArrays(1, &BlitVAO);

    if (Mat == nullptr)
        Mat = Material::New("Blit", "glsl_shaders/Blit.vert", "glsl_shaders/Blit.frag");
}

void Blitter::Cleanup()
{
    glDeleteVertexArrays(1, &BlitVAO);
}