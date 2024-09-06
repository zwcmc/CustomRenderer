#pragma once

#include <glad/glad.h>

#include "base/Texture2D.h"
#include "renderer/RenderTarget.h"
#include "base/Material.h"
#include "cameras/Camera.h"

class Blitter
{
public:
    static void BlitCameraTexture(const Texture2D::Ptr sourceTex, const RenderTarget::Ptr destination, Material::Ptr material = nullptr);
    static void BlitCameraTexture(const RenderTarget::Ptr source, const RenderTarget::Ptr destination, Material::Ptr material = nullptr);
    static void BlitCamera(const Texture2D::Ptr sourceTex, const Camera::Ptr camera, Material::Ptr material = nullptr);
    static void BlitCamera(const RenderTarget::Ptr source, const Camera::Ptr camera, Material::Ptr material = nullptr);
    
    static void RenderToTarget(const RenderTarget::Ptr target, Material::Ptr material);
    
    static void CopyDepth(const RenderTarget::Ptr source, const RenderTarget::Ptr destination);
    static void BlitDepth(const RenderTarget::Ptr source, const RenderTarget::Ptr destination);

    static void Init();
    static void Cleanup();
private:
    static void DrawFullScreenTriangle();
    
    static GLuint BlitVAO;
    static Material::Ptr DefaultBlitMat;
    static Material::Ptr BlitDepthMat;
};
