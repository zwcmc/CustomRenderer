#include "base/Texture.h"

Texture::Texture()
    : m_TextureName(""), m_TextureID(0), m_Size(glm::u32vec2(1)), m_InternalFormat(GL_RGBA), m_Format(GL_RGBA), m_Type(GL_UNSIGNED_BYTE),
        m_Target(GL_TEXTURE_2D)
{ }

Texture::~Texture()
{
    glActiveTexture(0);
    glDeleteTextures(1, &m_TextureID);
}

void Texture::bind(int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(m_Target, m_TextureID);
}