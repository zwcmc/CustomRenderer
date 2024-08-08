#include "base/Texture2D.h"

Texture2D::Texture2D(const std::string& name)
{
    m_TextureName = name;
}

void Texture2D::initTexture2D(unsigned int width, unsigned int height, GLenum internalFormat, GLenum format, void* data, bool useMipmap)
{
    m_Width = width;
    m_Height = height;
    m_InternalFormat = internalFormat;
    m_Format = format;

    if (m_TextureID == 0)
        glGenTextures(1, &m_TextureID);

    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
    if (useMipmap)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture2D::initTexture2D(ktxTexture* kTexture, bool useMipmap)
{
    m_Width = kTexture->baseWidth;
    m_Height = kTexture->baseHeight;
    m_InternalFormat = kTexture->glInternalformat;
    m_Format = kTexture->glFormat;

    if (m_TextureID == 0)
        glGenTextures(1, &m_TextureID);

    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    GLenum target, glerror;
    KTX_error_code result = ktxTexture_GLUpload(kTexture, &m_TextureID, &target, &glerror);

    if (result != KTX_SUCCESS)
    {
        std::cerr << "Create Texture2D from KTX file failed, texture name:  " << m_TextureName << ", error: " << result << std::endl;
    }

    if (useMipmap)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}