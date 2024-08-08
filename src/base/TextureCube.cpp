#include "base/TextureCube.h"

TextureCube::TextureCube(const std::string& name)
{
    m_TextureName = name;
    m_Target = GL_TEXTURE_CUBE_MAP;
}

void TextureCube::initTextureCube(ktxTexture* kTexture)
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
        std::cerr << "Create TextureCube from KTX file failed, texture name:  " << m_TextureName << ", error: " << result << std::endl;
    }

    glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}