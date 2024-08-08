#pragma once

#include <vector>
#include <glad/glad.h>

#include "ptr.h"
#include "base/Texture2D.h"

class RenderTarget
{
    SHARED_PTR(RenderTarget)
public:

    RenderTarget(glm::u32vec2 size, GLenum type = GL_UNSIGNED_BYTE, unsigned int colorAttachmentsNum = 1);

    Texture2D::Ptr getColorTexture(unsigned int index);

    void resize(glm::u32vec2 size);
    void bind();

private:
    GLuint m_FrameBufferID;
    GLenum m_Type;
    glm::u32vec2 m_Size;

    std::vector<Texture2D::Ptr> m_ColorAttachments;
    Texture2D::Ptr m_DepthStencilAttachment;
};