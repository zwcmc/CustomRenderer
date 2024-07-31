#include "base/Texture.h"

GLuint Texture::unitCount = 0;

Texture::Texture()
    : m_TextureName(""), m_TextureID(0), m_TextureUnit(0), m_TextureUnitIndex(0), m_Width(0), m_Height(0), m_Components(0)
{ }

Texture::~Texture()
{
    unbind();
    glActiveTexture(0);
    glDeleteTextures(1, &m_TextureID);
}

void Texture::bind()
{
    glActiveTexture(m_TextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

void Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::initTexture(const std::string& textureName, int width, int height, int components, GLenum format, void* data, bool useMipmap)
{
    m_TextureName = textureName;
    m_Width = width;
    m_Height = height;
    m_Components = components;

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

    m_TextureUnit = GL_TEXTURE0 + Texture::unitCount;
    m_TextureUnitIndex = Texture::unitCount;
    Texture::unitCount++;
}