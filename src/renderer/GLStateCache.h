#ifndef GL_STATE_CACHE_H
#define GL_STATE_CACHE_H

#include <glad/glad.h>

#include "ptr.h"

class GLStateCache
{
    SHARED_PTR(GLStateCache)
public:
    
    GLStateCache() = default;
    ~GLStateCache() = default;
    
    void InitState();
    
    void SetDepthTest(bool enable);
    void SetDepthWriteMask(GLenum mask);
    void SetDepthFunc(GLenum depthFunc);
    
    void SetBlend(bool enable);
    void SetBlendFactor(GLenum src, GLenum dst);
    
    void SetCull(bool enable);
    void SetCullFace(GLenum face);
    
    void SetPolygonMode(GLenum mode);
    
private:
    
    // Depth test
    bool m_DepthTest;
    // Alpha blend
    bool m_Blend;
    // Cull face
    bool m_CullFace;

    // Depth write, this only has effect if depth testing is enabled
    GLenum m_DepthWriteMask;
    // Depth test function
    GLenum m_DepthFunc;
    // Blend source factor
    GLenum m_BlendSrc;
    // Blend destination factor
    GLenum m_BlendDst;
    // Culled face
    GLenum m_CulledFace;
    // Polygon mode
    GLenum m_PolygonMode;
};

#endif
