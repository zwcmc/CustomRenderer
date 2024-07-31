#include "base/Material.h"

Material::Material(Shader* shader)
{
    m_Shader = shader;
}

void Material::addTexture(Texture* texture)
{
    m_Textures.push_back(texture);
}

void Material::setInt(const std::string &propertyName, const int value)
{
    m_Shader->setUniformInt(propertyName, value);
}

void Material::setMatrix(const std::string &propertyName, const glm::mat4x4 &value)
{
    m_Shader->setUniformMatrix(propertyName, value);
}

void Material::use()
{
    m_Shader->use();

    for (auto &texture : m_Textures)
    {
        m_Shader->setUniformInt(texture->getTextureName(), texture->getTextureUnitIndex());
        texture->bind();
    }
}