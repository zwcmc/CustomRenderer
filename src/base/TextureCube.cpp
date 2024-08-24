#include "base/TextureCube.h"

TextureCube::TextureCube(const std::string& name)
{
    m_TextureName = name;
    m_Target = GL_TEXTURE_CUBE_MAP;
}

void TextureCube::InitTextureCube(ktxTexture* kTexture, bool useMipmap)
{
    m_Size = glm::u32vec2(kTexture->baseWidth, kTexture->baseHeight);
    m_InternalFormat = kTexture->glInternalformat;
    m_Format = kTexture->glFormat;
    m_Type = kTexture->glType;

    if (m_TextureID == 0)
        glGenTextures(1, &m_TextureID);

    Bind();

    GLenum glerror;
    KTX_error_code result = ktxTexture_GLUpload(kTexture, &m_TextureID, &m_Target, &glerror);

    if (result != KTX_SUCCESS)
    {
        std::cerr << "Create TextureCube from KTX file failed, texture name:  " << m_TextureName << ", error: " << glerror << std::endl;
    }

    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    if (useMipmap)
    {
        glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(m_Target);
    }
    else
    {
        glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    Unbind();
}

void TextureCube::DefaultInit(const unsigned int &width, const unsigned int &height, GLenum internalFormat, GLenum format, GLenum type, bool useMipmap)
{
    m_Size = glm::u32vec2(width, height);
    m_InternalFormat = internalFormat;
    m_Format = format;
    m_Type = type;

    if (m_TextureID == 0)
        glGenTextures(1, &m_TextureID);

    Bind();

    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, type, nullptr);
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

    Unbind();
}