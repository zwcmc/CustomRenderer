#include "renderer/RenderTarget.h"

RenderTarget::RenderTarget(const unsigned int &width, const unsigned int &height, GLenum type, unsigned int colorAttachmentsNum, bool hasDepth, bool isShadowMap)
    : RenderTarget(glm::u32vec2(width, height), type, colorAttachmentsNum, hasDepth, isShadowMap)
{ }

RenderTarget::RenderTarget(const glm::u32vec2 &size, GLenum type, unsigned int colorAttachmentsNum, bool hasDepth, bool isShadowMap)
    : m_FrameBufferID(0), m_Size(size), m_Type(type), m_HasDepthAttachment(hasDepth), m_IsShadowMap(isShadowMap)
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
            Texture2D::Ptr colorAttachment = Texture2D::New("uColorAttachment" + std::to_string(i));
            colorAttachment->InitTexture2D(size, internalFormat, GL_RGBA, type, nullptr);
            colorAttachment->SetWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + static_cast<int>(i), GL_TEXTURE_2D, colorAttachment->GetTextureID(), 0);
            m_ColorAttachments.push_back(colorAttachment);
        }

        // Create depth attachment if needed
        if (hasDepth)
        {
            m_DepthAttachment = Texture2D::New("uDepthAttachment");
            m_DepthAttachment->InitTexture2D(size, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_HALF_FLOAT, nullptr);
            m_DepthAttachment->SetWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment->GetTextureID(), 0);
        }
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer not complete!" << std::endl;
    }

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::~RenderTarget()
{
    glDeleteFramebuffers(1, &m_FrameBufferID);
    m_ColorAttachments.clear();
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

Texture2D::Ptr RenderTarget::GetDepthTexture()
{
    return m_HasDepthAttachment ? m_DepthAttachment : nullptr;
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

    if (m_HasDepthAttachment)
    {
        m_DepthAttachment->SetSize(size);
    }
}

void RenderTarget::SetSize(const unsigned int &width, const unsigned int &height)
{
    m_Size = glm::u32vec2(width, height);
    
    for (unsigned int i = 0; i < m_ColorAttachments.size(); ++i)
    {
        m_ColorAttachments[i]->SetSize(m_Size);
    }

    if (m_HasDepthAttachment)
    {
        m_DepthAttachment->SetSize(m_Size);
    }
}

void RenderTarget::BindTarget(const bool &clearColor, const bool &clearDepth)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
    glViewport(0, 0, m_Size.x, m_Size.y);
    
    GLbitfield mask = 0;
    
    if (clearColor)
    {
        mask |= GL_COLOR_BUFFER_BIT;
    }
    if (clearDepth)
    {
        mask |= GL_DEPTH_BUFFER_BIT;
    }

    if (mask)
    {
        glClear(mask);
    }
}

void RenderTarget::UnbindTarget()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint& RenderTarget::GetFrameBufferID()
{
    return m_FrameBufferID;
}
