#include "base/Material.h"

#include "loader/AssetsLoader.h"

Material::Material(const std::string &shaderName, const std::string &vsPath, const std::string &fsPath, bool usedForSkybox)
    : m_DoubleSided(false), m_AlphaMode(AlphaMode::DEFAULT_OPAQUE), m_UsedForSkybox(usedForSkybox)
{
    m_Shader = AssetsLoader::loadShaderFromFile(shaderName, vsPath, fsPath);
}

Material::~Material()
{
    m_Textures.clear();
    m_TextureCubes.clear();
    m_UniformVec3.clear();
    m_UniformVec4.clear();
    m_UniformFloats.clear();
}

void Material::addOrSetTexture(Texture2D::Ptr texture)
{
    m_Textures.insert_or_assign(texture->getTextureName(), texture);
}

void Material::addOrSetTextureCube(TextureCube::Ptr textureCube)
{
    m_TextureCubes.insert_or_assign(textureCube->getTextureName(), textureCube);
}

void Material::addOrSetTexture(const std::string &propertyName, Texture2D::Ptr texture)
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

    if (m_Textures.size() > 0 || m_TextureCubes.size() > 0)
    {
        int unit = 0;
        for (auto &pair : m_TextureCubes)
        {
            m_Shader->setUniformInt(pair.first, unit);
            pair.second->bind(unit);
            ++unit;
        }
        for (auto &pair : m_Textures)
        {
            m_Shader->setUniformInt(pair.first, unit);
            pair.second->bind(unit);
            ++unit;
        }
    }
}