#pragma once

#include <vector>
#include <glad/glad.h>

#include "ptr.h"
#include "base/Texture2D.h"

class RenderTarget
{
    SHARED_PTR(RenderTarget)
public:
    RenderTarget(const unsigned int &width, const unsigned int &height, GLenum type = GL_UNSIGNED_BYTE, unsigned int colorAttachmentsNum = 1,
        bool depthAndStencil = false);
    RenderTarget(const glm::u32vec2 &size, GLenum type = GL_UNSIGNED_BYTE, unsigned int colorAttachmentsNum = 1, bool depthAndStencil = false);

    Texture2D::Ptr getColorTexture(const unsigned int &index);
    Texture2D::Ptr getDepthTexture();

    glm::u32vec2& getSize() { return m_Size; }
    void resize(const glm::u32vec2 &size);
    void bind();

private:
    GLuint m_FrameBufferID;
    GLenum m_Type;
    glm::u32vec2 m_Size;

    bool m_HasDepthAndStencil;
    std::vector<Texture2D::Ptr> m_ColorAttachments;
    Texture2D::Ptr m_DepthStencilAttachment;
};