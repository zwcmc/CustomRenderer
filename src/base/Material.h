#pragma once

#include <vector>
#include <map>

#include <memory>

#include "ptr.h"
#include "base/Shader.h"
#include "base/Texture.h"

class Material
{
    SHARED_PTR(Material)
public:

    struct MaterialData
    {
        using Ptr = std::shared_ptr<MaterialData>;
        inline static Ptr New()
        {
            return std::make_shared<MaterialData>();
        }

        float albedoMapSet;
        Texture::Ptr albedoMap;
        glm::vec4 baseColor;

        MaterialData() : albedoMapSet(-1.0f), baseColor(glm::vec4(1.0f)) { }
    };
    

    Material(Shader::Ptr shader);
    ~Material();

    void setShader(Shader::Ptr shader);

    void addTextureProperty(Texture::Ptr texture);
    void addTextureProperty(const std::string &propertyName, Texture::Ptr texture);
    void addVectorProperty(const std::string &propertyName, const glm::vec4 &value);
    void addFloatProperty(const std::string &propertyName, const float &value);
    void setMatrix(const std::string &propertyName, const glm::mat3x3 &value);
    void setMatrix(const std::string &propertyName, const glm::mat4x4& value);

    void use();
private:
    Shader::Ptr m_Shader;

    std::vector<Texture::Ptr> m_Textures;
    std::map<std::string, glm::vec4> m_UniformVectors;
    std::map<std::string, float> m_UniformFloats;
};