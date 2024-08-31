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

void Blitter::BlitToCameraTarget(const RenderTarget::Ptr source, const Camera::Ptr targetCamera, Material::Ptr blitMat)
{
    assert(source != nullptr);

    targetCamera->BindCameraTarget();

    if (blitMat == nullptr)
        blitMat = Mat;

    // Setup source texture
    source->GetColorTexture(0)->SetTextureName("uSource");
    blitMat->AddOrSetTexture(source->GetColorTexture(0));
    // Setup source texture texel size
    glm::u32vec2 textureSize = source->GetColorTexture(0)->GetSize();
    blitMat->AddOrSetVector("uSourceTexelSize", glm::vec4(1.0f / textureSize.x, 1.0f / textureSize.y, textureSize.x, textureSize.y));

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
