#include "renderer/RenderTarget.h"

RenderTarget::RenderTarget(unsigned int width, unsigned int height, GLenum type, unsigned int colorAttachmentsNum)
    : RenderTarget(glm::u32vec2(width, height), type, colorAttachmentsNum)
{ }

RenderTarget::RenderTarget(glm::u32vec2 size, GLenum type, unsigned int colorAttachmentsNum)
    : m_FrameBufferID(0), m_Size(size), m_Type(type)
{
    glGenFramebuffers(1, &m_FrameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);

    // Color attachments
    GLenum internalFormat = GL_RGBA;
    if (type == GL_HALF_FLOAT)
    {
        internalFormat = GL_RGBA16F; // Using GL_RGBA16F for HDR will suffice
    }
    else if (type == GL_FLOAT)
    {
        internalFormat = GL_RGBA32F;
    }
    for (size_t i = 0; i < colorAttachmentsNum; i++)
    {
        Texture2D::Ptr colorAttachment = Texture2D::New("_ColorAttachment" + std::to_string(i));
        colorAttachment->setWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        colorAttachment->initTexture2D(size, internalFormat, GL_RGBA, type, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + static_cast<int>(i), GL_TEXTURE_2D, colorAttachment->getTextureID(), 0);
        m_ColorAttachments.push_back(colorAttachment);
    }

    // Depth stencil attachment
    {
        Texture2D::Ptr depthStencilAttachment = Texture2D::New("_DepthStencilAttachment");
        depthStencilAttachment->setWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        depthStencilAttachment->initTexture2D(size, GL_DEPTH_STENCIL, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilAttachment->getTextureID(), 0);
        m_DepthStencilAttachment = depthStencilAttachment;
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Texture2D::Ptr RenderTarget::getColorTexture(unsigned int index)
{
    if (index < m_ColorAttachments.size())
    {
        return m_ColorAttachments[index];
    }
    else
    {
        std::cerr << "Error, index: " << index << ", size: " << m_ColorAttachments.size() << std::endl;
        return nullptr;
    }
}

void RenderTarget::resize(glm::u32vec2 size)
{
    m_Size = size;

    for (unsigned int i = 0; i < m_ColorAttachments.size(); ++i)
    {
        m_ColorAttachments[i]->resize(size);
    }

    m_DepthStencilAttachment->resize(size);
}

void RenderTarget::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
    glViewport(0, 0, m_Size.x, m_Size.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}