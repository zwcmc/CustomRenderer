#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>

#include "ptr.h"
#include "base/Shader.h"
#include "base/Texture.h"

class Material
{
    SHARED_PTR(Material)
public:
    enum AlphaMode
    {
        OPAQUE,
        MASK,
        BLEND
    };

    Material(const std::string &shaderName, const std::string &vsPath, const std::string &fsPath);
    ~Material();

    void addTextureProperty(Texture::Ptr texture);
    void addTextureProperty(const std::string &propertyName, Texture::Ptr texture);
    void addVectorProperty(const std::string &propertyName, const glm::vec3 &value);
    void addVectorProperty(const std::string &propertyName, const glm::vec4 &value);
    void addFloatProperty(const std::string &propertyName, const float &value);
    void setMatrix(const std::string &propertyName, const glm::mat3x3 &value);
    void setMatrix(const std::string &propertyName, const glm::mat4x4& value);

    void setDoubleSided(bool bDoubleSided);
    bool getDoubleSided() { return m_DoubleSided; }

    void setAlphaMode(AlphaMode mode);
    AlphaMode getAlphaMode() { return m_AlphaMode; }

    void use();
private:
    Shader::Ptr m_Shader;

    std::vector<Texture::Ptr> m_Textures;
    std::map<std::string, glm::vec3> m_UniformVec3;
    std::map<std::string, glm::vec4> m_UniformVec4;
    std::map<std::string, float> m_UniformFloats;

    bool m_DoubleSided;
    AlphaMode m_AlphaMode;
};