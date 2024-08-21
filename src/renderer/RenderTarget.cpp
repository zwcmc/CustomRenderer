#include "renderer/RenderTarget.h"

RenderTarget::RenderTarget(const unsigned int &width, const unsigned int &height, GLenum type, unsigned int colorAttachmentsNum, bool depthAndStencil, bool isShadowmap)
    : RenderTarget(glm::u32vec2(width, height), type, colorAttachmentsNum, depthAndStencil, isShadowmap)
{ }

RenderTarget::RenderTarget(const glm::u32vec2 &size, GLenum type, unsigned int colorAttachmentsNum, bool depthAndStencil, bool isShadowmap)
    : m_FrameBufferID(0), m_Size(size), m_Type(type), m_HasDepthAndStencil(depthAndStencil), m_IsShadowmap(isShadowmap)
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

    if (isShadowmap)
    {
        m_ShadowmapAttachment = Texture2D::New("uShadowmap");
        m_ShadowmapAttachment->initShadowmap(size);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowmapAttachment->getTextureID(), 0);

        // Disable writes to the color buffer
        glDrawBuffer(GL_NONE);
        // Disable reads from the color buffer
        glReadBuffer(GL_NONE);
    }
    else
    {
        for (size_t i = 0; i < colorAttachmentsNum; i++)
        {
            Texture2D::Ptr colorAttachment = Texture2D::New("_ColorAttachment" + std::to_string(i));
            colorAttachment->initTexture2D(size, internalFormat, GL_RGBA, type, nullptr);
            colorAttachment->setWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + static_cast<int>(i), GL_TEXTURE_2D, colorAttachment->getTextureID(), 0);
            m_ColorAttachments.push_back(colorAttachment);
        }

        // Create depth stencil attachment if needed
        m_HasDepthAndStencil = depthAndStencil;
        if (depthAndStencil)
        {
            m_DepthStencilAttachment = Texture2D::New("_DepthStencilAttachment");
            m_DepthStencilAttachment->initTexture2D(size, GL_DEPTH_STENCIL, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
            m_DepthStencilAttachment->setWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthStencilAttachment->getTextureID(), 0);
        }
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Texture2D::Ptr RenderTarget::getColorTexture(const unsigned int &index)
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

Texture2D::Ptr RenderTarget::getDepthAndStencilTexture()
{
    return m_HasDepthAndStencil ? m_DepthStencilAttachment : nullptr;
}

Texture2D::Ptr RenderTarget::getShadowmapTexture()
{
    return m_IsShadowmap ? m_ShadowmapAttachment : nullptr;
}

void RenderTarget::resize(const glm::u32vec2 &size)
{
    m_Size = size;

    for (unsigned int i = 0; i < m_ColorAttachments.size(); ++i)
    {
        m_ColorAttachments[i]->resize(size);
    }

    if (m_HasDepthAndStencil)
    {
        m_DepthStencilAttachment->resize(size);
    }
}

void RenderTarget::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
    glViewport(0, 0, m_Size.x, m_Size.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}