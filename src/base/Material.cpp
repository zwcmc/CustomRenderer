#include "base/Material.h"

#include "loader/AssetsLoader.h"

Material::Material(const std::string &shaderName, const std::string &vsPath, const std::string &fsPath, bool usedForSkybox)
    : m_DoubleSided(false), m_AlphaMode(AlphaMode::DEFAULT_OPAQUE), m_UsedForSkybox(usedForSkybox), m_CastShadows(true)
{
    m_Shader = AssetsLoader::LoadShader(shaderName, vsPath, fsPath);
    
    if (m_UsedForSkybox)
        m_DoubleSided = true;
}

Material::~Material()
{
    m_Textures.clear();
    m_TextureCubes.clear();
    m_UniformVec3.clear();
    m_UniformVec4.clear();
    m_UniformFloats.clear();
}

void Material::AddOrSetTexture(Texture2D::Ptr texture)
{
    m_Textures.insert_or_assign(texture->GetTextureName(), texture);
}

void Material::AddOrSetTextureCube(TextureCube::Ptr textureCube)
{
    m_TextureCubes.insert_or_assign(textureCube->GetTextureName(), textureCube);
}

void Material::AddOrSetTexture(const std::string &propertyName, Texture2D::Ptr texture)
{
    texture->SetTextureName(propertyName);
    m_Textures.insert_or_assign(propertyName, texture);
}

void Material::AddOrSetVector(const std::string &propertyName, const glm::vec3 &value)
{
    m_UniformVec3.insert_or_assign(propertyName, value);
}

void Material::AddOrSetVector(const std::string &propertyName, const glm::vec4 &value)
{
    m_UniformVec4.insert_or_assign(propertyName, value);
}

void Material::AddOrSetFloat(const std::string &propertyName, const float &value)
{
    m_UniformFloats.insert_or_assign(propertyName, value);
}

void Material::SetMatrix(const std::string &propertyName, const glm::mat3x3 &value)
{
    m_Shader->SetUniformMatrix(propertyName, value);
}

void Material::SetMatrix(const std::string &propertyName, const glm::mat4x4 &value)
{
    m_Shader->SetUniformMatrix(propertyName, value);
}

void Material::SetDoubleSided(bool bDoubleSided)
{
    m_DoubleSided = bDoubleSided;
}

void Material::SetAlphaMode(AlphaMode mode)
{
    m_AlphaMode = mode;
}

void Material::Use()
{
    m_Shader->Use();

    if (m_UniformVec3.size() > 0)
    {
        for (auto &pair : m_UniformVec3)
        {
            m_Shader->SetUniformVector(pair.first, pair.second);
        }
    }

    if (m_UniformVec4.size() > 0)
    {
        for (auto &pair : m_UniformVec4)
        {
            m_Shader->SetUniformVector(pair.first, pair.second);
        }
    }

    if (m_UniformFloats.size() > 0)
    {
        for (auto &pair : m_UniformFloats)
        {
            m_Shader->SetUniformFloat(pair.first, pair.second);
        }
    }

    if (m_Textures.size() > 0 || m_TextureCubes.size() > 0)
    {
        int unit = 0;
        for (auto &pair : m_Textures)
        {
            m_Shader->SetUniformInt(pair.first, unit);
            pair.second->Bind(unit);
            ++unit;
        }
        for (auto &pair : m_TextureCubes)
        {
            m_Shader->SetUniformInt(pair.first, unit);
            pair.second->Bind(unit);
            ++unit;
        }
    }
}
