#include "renderer/PostProcessing.h"

#include "renderer/Blitter.h"

PostProcessing::PostProcessing()
{
    m_FXAAMat = Material::New("FXAA", "glsl_shaders/Blit.vert", "glsl_shaders/post_processing/FXAA.frag");
}

void PostProcessing::Render(const RenderTarget::Ptr source, const Camera::Ptr targetCamera)
{
    // Fast Approximate Anti-aliasing
    Blitter::BlitToCameraTarget(source, targetCamera, m_FXAAMat);
}
