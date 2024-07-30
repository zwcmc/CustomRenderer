#include "base/texture/Texture.h"

#include <stb_image.h>

#include "tool/AssetsLoader.h"

GLuint Texture::unitCount = 0;

Texture::Texture()
    : m_TextureName(""), m_TextureID(0), m_TextureUnit(0), m_TextureUnitIndex(0), m_Width(0), m_Height(0), m_Components(0), m_FlipVertical(false), m_UseMipmap(false), m_FilePath("")
{ }

Texture::Texture(const std::string &textureName, const std::string &filePath, bool flipVertical, bool useMipmap)
    : m_TextureName(textureName), m_TextureID(0), m_TextureUnit(0), m_TextureUnitIndex(0), m_Width(0), m_Height(0), m_Components(0), m_FlipVertical(flipVertical), m_UseMipmap(useMipmap), m_FilePath(filePath)
{
    generateTextureFromFile(filePath);
}

Texture::~Texture()
{
    unbind();
    glActiveTexture(0);
    glDeleteTextures(1, &m_TextureID);
}

void Texture::generateTextureFromFile(const std::string &filePath)
{
    if (filePath.empty())
        return;
    
    stbi_set_flip_vertically_on_load(m_FlipVertical);

    std::string newPath = AssetsLoader::getAssetsPath() + filePath;
    std::cout << newPath << std::endl;
    unsigned char *data = stbi_load(newPath.c_str(), &m_Width, &m_Height, &m_Components, 0);
    if (!data)
    {
        std::cerr << "Failed to load texture: " + newPath << std::endl;
        stbi_image_free(data);
        return;
    }

    glGenTextures(1, &m_TextureID);
    initTexture(data);
    m_TextureUnit = GL_TEXTURE0 + unitCount;
    m_TextureUnitIndex = unitCount;
    unitCount++;

    stbi_image_free(data);
}

void Texture::initTexture(unsigned char* buffer)
{
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    GLenum format = 0;
    switch (m_Components)
    {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (m_UseMipmap)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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