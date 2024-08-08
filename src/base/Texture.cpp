#include "base/Texture.h"

Texture::Texture()
    : m_TextureName(""), m_TextureID(0), m_Width(1), m_Height(1), m_InternalFormat(GL_RGBA), m_Format(GL_RGBA)
{ }

Texture::~Texture()
{
    glActiveTexture(0);
    glDeleteTextures(1, &m_TextureID);
}