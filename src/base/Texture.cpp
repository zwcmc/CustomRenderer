#include "base/Texture.h"

Texture::Texture()
    : m_TextureName(""), m_TextureID(0), m_Width(0), m_Height(0), m_Components(0)
{ }

Texture::~Texture()
{
    glActiveTexture(0);
    glDeleteTextures(1, &m_TextureID);
}

void Texture::initTexture(const std::string& textureName, int width, int height, int components, GLenum format, void* data, bool useMipmap)
{
    m_TextureName = textureName;
    m_Width = width;
    m_Height = height;
    m_Components = components;

    if (!m_TextureID)
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

    glBindTexture(GL_TEXTURE_2D, 0);
}