#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "base/Texture2D.h"
#include "renderer/RenderTarget.h"
#include "base/Material.h"

class Blitter
{
public:
    static void BlitToTarget(Texture2D::Ptr source, RenderTarget::Ptr dest, Material::Ptr blitMat = nullptr);
    static void BlitToCamera(Texture2D::Ptr source, glm::u32vec2 size, Material::Ptr blitMat = nullptr);
    static void RenderToTarget(RenderTarget::Ptr target, Material::Ptr blitMat = nullptr);

    static void Init();
    static void Cleanup();

    static GLuint BlitVAO;
    static Material::Ptr Mat;
};