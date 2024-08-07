#include "base/TextureCube.h"

void TextureCube::initTextureCube(const std::string &textureName, ktxTexture* kTexture)
{
    m_TextureName = textureName;
    m_Width = kTexture->baseWidth;
    m_Height = kTexture->baseHeight;

    if (m_TextureID == 0)
        glGenTextures(1, &m_TextureID);

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

    GLenum target, glerror;
    KTX_error_code result = ktxTexture_GLUpload(kTexture, &m_TextureID, &target, &glerror);

    if (result != KTX_SUCCESS)
    {
        std::cerr << "Create TextureCube from KTX file failed, texture name:  " << textureName << ", error: " << result << std::endl;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}