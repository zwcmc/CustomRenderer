#include "base/Texture2D.h"

Texture2D::Texture2D(const std::string &name)
{
    m_TextureName = name;
    m_Target = GL_TEXTURE_2D;
}

void Texture2D::SetFilterMode(GLenum minFilter, GLenum magFilter)
{
    Bind();

    glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, magFilter);

    Unbind();
}

void Texture2D::SetWrapMode(GLenum wrapS, GLenum wrapT)
{
    Bind();

    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, wrapT);

    Unbind();
}

void Texture2D::SetSize(const glm::u32vec2 &size)
{
    m_Size = size;

    Bind();
    if (m_Target == GL_TEXTURE_2D)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, size.x, size.y, 0, m_Format, m_Type, 0);
    }
}

void Texture2D::InitTexture2D(const glm::u32vec2 &size, GLenum internalFormat, GLenum format, GLenum type, void* data, bool useMipmap)
{
    m_Size = size;
    m_InternalFormat = internalFormat;
    m_Format = format;
    m_Type = type;

    glGenTextures(1, &m_TextureID);

    Bind();
    
    if (useMipmap)
    {
        glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    else
    {
        glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(m_Target, 0, m_InternalFormat, size.x, size.y, 0, format, type, data);
    
    if (useMipmap)
    {
        glGenerateMipmap(m_Target);
    }

    Unbind();
}

void Texture2D::InitShadowMap(const glm::u32vec2 &size)
{
    m_Size = size;
    m_InternalFormat = GL_DEPTH_COMPONENT32; // Fixed-point format for range [0, 1]
    m_Format = GL_DEPTH_COMPONENT;
    m_Type = GL_FLOAT;

    glGenTextures(1, &m_TextureID);

    Bind();
    
    glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(m_Target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(m_Target, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glTexImage2D(m_Target, 0, m_InternalFormat, m_Size.x, m_Size.y, 0, m_Format, m_Type, nullptr);

    Unbind();
}
