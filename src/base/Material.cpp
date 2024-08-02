#include "base/Material.h"

Material::Material(Shader::Ptr shader)
{
    m_Shader = shader;
}

Material::~Material()
{
    m_Textures.clear();
    m_UniformVectors.clear();
    m_UniformFloats.clear();
}

void Material::setShader(Shader::Ptr shader)
{
    m_Shader = shader;
}

void Material::addTextureProperty(Texture::Ptr texture)
{
    m_Textures.push_back(texture);
}

void Material::addTextureProperty(const std::string &propertyName, Texture::Ptr texture)
{
    texture->setTextureName(propertyName);
    m_Textures.push_back(texture);
}

void Material::addVectorProperty(const std::string &propertyName, const glm::vec4& value)
{
    m_UniformVectors.insert(std::make_pair(propertyName, value));
}

void Material::addFloatProperty(const std::string &propertyName, const float &value)
{
    m_UniformFloats.insert(std::make_pair(propertyName, value));
}

void Material::setMatrix(const std::string &propertyName, const glm::mat3x3 &value)
{
    m_Shader->setUniformMatrix(propertyName, value);
}

void Material::setMatrix(const std::string &propertyName, const glm::mat4x4 &value)
{
    m_Shader->setUniformMatrix(propertyName, value);
}

void Material::use()
{
    m_Shader->use();

    if (m_UniformVectors.size() > 0)
    {
        for (auto &pair : m_UniformVectors)
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

    int slot = 0;
    for (auto &texture : m_Textures)
    {
        m_Shader->setUniformInt(texture->getTextureName(), slot);
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
        ++slot;
    }
}