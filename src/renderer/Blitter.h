#pragma once

#include <glad/glad.h>

#include "base/Texture2D.h"
#include "renderer/RenderTarget.h"
#include "base/Material.h"
#include "cameras/Camera.h"

class Blitter
{
public:
    static void BlitToTarget(const Texture2D::Ptr sourceTex, const RenderTarget::Ptr target, Material::Ptr blitMat = nullptr);
    static void BlitToTarget(const RenderTarget::Ptr source, const RenderTarget::Ptr target, Material::Ptr blitMat = nullptr);
    static void BlitToCameraTarget(const RenderTarget::Ptr source, const Camera::Ptr targetCamera, Material::Ptr blitMat = nullptr);
    static void RenderToTarget(const RenderTarget::Ptr target, Material::Ptr blitMat = nullptr);

    static void Init();
    static void Cleanup();

    static GLuint BlitVAO;
    static Material::Ptr Mat;
};
