#include "base/Texture2D.h"

Texture2D::Texture2D(const std::string& name)
    : m_WarpS(GL_REPEAT), m_WarpT(GL_REPEAT)
{
    m_TextureName = name;
    m_Target = GL_TEXTURE_2D;
}

void Texture2D::setWrapMode(GLenum wrapS, GLenum wrapT)
{
    m_WarpS = wrapS;
    m_WarpT = wrapT;
}

void Texture2D::initTexture2D(glm::u32vec2 size, GLenum internalFormat, GLenum format, GLenum type, void* data, bool useMipmap)
{
    m_Size = size;
    m_InternalFormat = internalFormat;
    m_Format = format;
    m_Type = type;

    if (m_TextureID == 0)
        glGenTextures(1, &m_TextureID);

    glBindTexture(m_Target, m_TextureID);
    glTexImage2D(m_Target, 0, format, size.x, size.y, 0, format, type, data);
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

    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, m_WarpS);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, m_WarpT);
}

void Texture2D::initTexture2D(ktxTexture* kTexture, bool useMipmap)
{
    m_Size = glm::u32vec2(kTexture->baseWidth, kTexture->baseHeight);
    m_InternalFormat = kTexture->glInternalformat;
    m_Format = kTexture->glFormat;
    m_Type = kTexture->glType;

    if (m_TextureID == 0)
        glGenTextures(1, &m_TextureID);

    glBindTexture(m_Target, m_TextureID);
    GLenum target, glerror;
    KTX_error_code result = ktxTexture_GLUpload(kTexture, &m_TextureID, &target, &glerror);

    if (result != KTX_SUCCESS)
    {
        std::cerr << "Create Texture2D from KTX file failed, texture name:  " << m_TextureName << ", error: " << result << std::endl;
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

    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, m_WarpS);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, m_WarpT);
}