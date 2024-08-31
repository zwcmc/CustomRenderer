#pragma once

#include <glad/glad.h>

#include "base/Texture2D.h"
#include "renderer/RenderTarget.h"
#include "base/Material.h"
#include "cameras/Camera.h"

class Blitter
{
public:
    static void BlitToTarget(Texture2D::Ptr source, RenderTarget::Ptr dest, Material::Ptr blitMat = nullptr);
    static void BlitToCameraTarget(const RenderTarget::Ptr source, const Camera::Ptr targetCamera, Material::Ptr blitMat = nullptr);
    static void RenderToTarget(RenderTarget::Ptr target, Material::Ptr blitMat = nullptr);

    static void Init();
    static void Cleanup();

    static GLuint BlitVAO;
    static Material::Ptr Mat;
};
