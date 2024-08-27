#include "renderer/RenderTarget.h"

RenderTarget::RenderTarget(const unsigned int &width, const unsigned int &height, GLenum type, unsigned int colorAttachmentsNum, bool depthAndStencil, bool isShadowMap)
    : RenderTarget(glm::u32vec2(width, height), type, colorAttachmentsNum, depthAndStencil, isShadowMap)
{ }

RenderTarget::RenderTarget(const glm::u32vec2 &size, GLenum type, unsigned int colorAttachmentsNum, bool depthAndStencil, bool isShadowMap)
    : m_FrameBufferID(0), m_Size(size), m_Type(type), m_HasDepthAndStencil(depthAndStencil), m_IsShadowMap(isShadowMap)
{
    glGenFramebuffers(1, &m_FrameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);

    // Color attachments
    GLenum internalFormat = GL_RGBA;
    if (type == GL_HALF_FLOAT)
        internalFormat = GL_RGBA16F; // Using GL_RGBA16F for HDR will suffice
    else if (type == GL_FLOAT)
        internalFormat = GL_RGBA32F;

    if (isShadowMap)
    {
        m_ShadowMapAttachment = Texture2D::New("uShadowMap");
        m_ShadowMapAttachment->InitShadowMap(size);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowMapAttachment->GetTextureID(), 0);

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
            colorAttachment->InitTexture2D(size, internalFormat, GL_RGBA, type, nullptr);
            colorAttachment->SetWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + static_cast<int>(i), GL_TEXTURE_2D, colorAttachment->GetTextureID(), 0);
            m_ColorAttachments.push_back(colorAttachment);
        }

        // Create depth stencil attachment if needed
        m_HasDepthAndStencil = depthAndStencil;
        if (depthAndStencil)
        {
            m_DepthStencilAttachment = Texture2D::New("_DepthStencilAttachment");
            m_DepthStencilAttachment->InitTexture2D(size, GL_DEPTH_STENCIL, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
            m_DepthStencilAttachment->SetWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthStencilAttachment->GetTextureID(), 0);
        }
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Texture2D::Ptr RenderTarget::GetColorTexture(const unsigned int &index)
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

Texture2D::Ptr RenderTarget::GetDepthAndStencilTexture()
{
    return m_HasDepthAndStencil ? m_DepthStencilAttachment : nullptr;
}

Texture2D::Ptr RenderTarget::GetShadowMapTexture()
{
    return m_IsShadowMap ? m_ShadowMapAttachment : nullptr;
}

void RenderTarget::SetSize(const glm::u32vec2 &size)
{
    m_Size = size;

    for (unsigned int i = 0; i < m_ColorAttachments.size(); ++i)
    {
        m_ColorAttachments[i]->SetSize(size);
    }

    if (m_HasDepthAndStencil)
    {
        m_DepthStencilAttachment->SetSize(size);
    }
}

void RenderTarget::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
    glViewport(0, 0, m_Size.x, m_Size.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
