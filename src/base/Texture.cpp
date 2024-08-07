#include "base/Texture.h"

Texture::Texture()
    : m_TextureName(""), m_TextureID(0), m_Width(0), m_Height(0)
{ }

Texture::~Texture()
{
    glActiveTexture(0);
    glDeleteTextures(1, &m_TextureID);
}