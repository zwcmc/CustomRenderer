#include "renderer/GLStateCache.h"

void GLStateCache::InitState()
{
    // No seams at cubemap edges
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    SetDepthTest(true);
    SetDepthFunc(GL_LESS);
    
    SetBlend(false);
    SetBlendFactor(GL_ONE, GL_ZERO);

    SetCull(true);
    SetCullFace(GL_BACK);
    
    SetPolygonMode(GL_FILL);
}

void GLStateCache::SetDepthTest(bool enable)
{
    if (m_DepthTest != enable)
    {
        m_DepthTest = enable;
        if (enable)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
    }
}

void GLStateCache::SetDepthWriteMask(GLenum mask)
{
    if (m_DepthWriteMask != mask)
    {
        m_DepthWriteMask = mask;
        glDepthMask(mask);
    }
}

void GLStateCache::SetDepthFunc(GLenum depthFunc)
{
    if (m_DepthFunc != depthFunc)
    {
        m_DepthFunc = depthFunc;
        glDepthFunc(depthFunc);
    }
}

void GLStateCache::SetBlend(bool enable)
{
    if (m_Blend != enable)
    {
        m_Blend = enable;
        if (enable)
        {
            glEnable(GL_BLEND);
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }
}

void GLStateCache::SetBlendFactor(GLenum src, GLenum dst)
{
    if (m_BlendSrc != src || m_BlendDst != dst)
    {
        m_BlendSrc = src;
        m_BlendDst = dst;
        glBlendFunc(src, dst);
    }
}

void GLStateCache::SetCull(bool enable)
{
    if (m_CullFace != enable)
    {
        m_CullFace = enable;
        if (enable)
        {
            glEnable(GL_CULL_FACE);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
    }
}

void GLStateCache::SetCullFace(GLenum face)
{
    if (m_CulledFace != face)
    {
        m_CulledFace = face;
        glCullFace(face);
    }
}

void GLStateCache::SetPolygonMode(GLenum mode)
{
    if (m_PolygonMode != mode)
    {
        m_PolygonMode = mode;
        glPolygonMode(GL_FRONT_AND_BACK, mode);
    }
}
