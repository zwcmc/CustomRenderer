#include "base/Material.h"

#include "loader/AssetsLoader.h"

Material::Material(const std::string &shaderName, const std::string &vsPath, const std::string &fsPath)
    : m_DoubleSided(false), m_AlphaMode(AlphaMode::DEFAULT_OPAQUE)
{
    m_Shader = AssetsLoader::loadShaderFromFile(shaderName, vsPath, fsPath);
}

Material::~Material()
{
    m_Textures.clear();
    m_UniformVec3.clear();
    m_UniformVec4.clear();
    m_UniformFloats.clear();
}

void Material::addOrSetTexture(Texture::Ptr texture)
{
    m_Textures.insert_or_assign(texture->getTextureName(), texture);
}

void Material::addOrSetTexture(const std::string &propertyName, Texture::Ptr texture)
{
    texture->setTextureName(propertyName);
    m_Textures.insert_or_assign(propertyName, texture);
}

void Material::addOrSetVector(const std::string &propertyName, const glm::vec3 &value)
{
    m_UniformVec3.insert_or_assign(propertyName, value);
}

void Material::addOrSetVector(const std::string &propertyName, const glm::vec4 &value)
{
    m_UniformVec4.insert_or_assign(propertyName, value);
}

void Material::addOrSetFloat(const std::string &propertyName, const float &value)
{
    m_UniformFloats.insert_or_assign(propertyName, value);
}

void Material::setMatrix(const std::string &propertyName, const glm::mat3x3 &value)
{
    m_Shader->setUniformMatrix(propertyName, value);
}

void Material::setMatrix(const std::string &propertyName, const glm::mat4x4 &value)
{
    m_Shader->setUniformMatrix(propertyName, value);
}

void Material::setDoubleSided(bool bDoubleSided)
{
    m_DoubleSided = bDoubleSided;
}

void Material::setAlphaMode(AlphaMode mode)
{
    m_AlphaMode = mode;
}

void Material::use()
{
    m_Shader->use();

    if (m_UniformVec3.size() > 0)
    {
        for (auto &pair : m_UniformVec3)
        {
            m_Shader->setUniformVector(pair.first, pair.second);
        }
    }

    if (m_UniformVec4.size() > 0)
    {
        for (auto &pair : m_UniformVec4)
        {
            m_Shader->setUniformVector(pair.first, pair.second);
        }
    }

    if (m_UniformFloats.size() > 0)
    {
        for (auto &pair : m_UniformFloats)
        {
            m_Shader->setUniformFloat(pair.first, pair.second);
        }
    }

    if (m_Textures.size() > 0)
    {
        int slot = 0;
        for (auto &pair : m_Textures)
        {
            std::cout << pair.first << ", " << slot << std::endl;
            m_Shader->setUniformInt(pair.first, slot);
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, pair.second->getTextureID());
            ++slot;
        }
    }
}