#include "base/TextureCube.h"

TextureCube::TextureCube(const std::string& name)
{
    m_TextureName = name;
    m_Target = GL_TEXTURE_CUBE_MAP;
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

    if (useMipmap)
    {
        glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    else
    {
        glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
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
        glGenerateMipmap(m_Target);
    }

    Unbind();
}
