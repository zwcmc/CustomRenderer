#include "base/Texture2D.h"

Texture2D::Texture2D(const std::string &name)
{
    m_TextureName = name;
    m_Target = GL_TEXTURE_2D;
}

void Texture2D::setFilterMode(GLenum minFilter, GLenum magFilter)
{
    bind();

    glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, magFilter);

    unbind();
}

void Texture2D::setWrapMode(GLenum wrapS, GLenum wrapT)
{
    bind();

    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, wrapT);

    unbind();
}

void Texture2D::resize(const glm::u32vec2 &size)
{
    bind();
    if (m_Target == GL_TEXTURE_2D)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, size.x, size.y, 0, m_Format, m_Type, 0);
    }
}

void Texture2D::initTexture2D(const glm::u32vec2 &size, GLenum internalFormat, GLenum format, GLenum type, void* data, bool useMipmap)
{
    m_Size = size;
    m_InternalFormat = internalFormat;
    m_Format = format;
    m_Type = type;

    glGenTextures(1, &m_TextureID);

    bind();

    glTexImage2D(m_Target, 0, m_InternalFormat, size.x, size.y, 0, format, type, data);

    if (useMipmap)
    {
        glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(m_Target);
    }
    else
    {
        glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_REPEAT);

    unbind();
}

void Texture2D::initTexture2D(ktxTexture* kTexture, bool useMipmap)
{
    m_Size = glm::u32vec2(kTexture->baseWidth, kTexture->baseHeight);
    m_InternalFormat = kTexture->glInternalformat;
    m_Format = kTexture->glFormat;
    m_Type = kTexture->glType;

    glGenTextures(1, &m_TextureID);

    bind();

    GLenum glerror;
    KTX_error_code result = ktxTexture_GLUpload(kTexture, &m_TextureID, &m_Target, &glerror);

    if (result != KTX_SUCCESS)
    {
        std::cerr << "Create Texture2D from KTX file failed, texture name:  " << m_TextureName << ", error: " << glerror << std::endl;
    }

    if (useMipmap)
    {
        glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(m_Target);
    }
    else
    {
        glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_REPEAT);

    unbind();
}