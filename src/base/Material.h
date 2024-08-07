#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>

#include "ptr.h"
#include "base/Shader.h"
#include "base/Texture2D.h"

class Material
{
    SHARED_PTR(Material)
public:
    enum AlphaMode
    {
        DEFAULT_OPAQUE = 0,
        MASK,
        BLEND
    };

    Material(const std::string &shaderName, const std::string &vsPath, const std::string &fsPath);
    ~Material();

    void addOrSetTexture(Texture2D::Ptr texture);
    void addOrSetTexture(const std::string &propertyName, Texture2D::Ptr texture);
    void addOrSetVector(const std::string &propertyName, const glm::vec3 &value);
    void addOrSetVector(const std::string &propertyName, const glm::vec4 &value);
    void addOrSetFloat(const std::string &propertyName, const float &value);
    void setMatrix(const std::string &propertyName, const glm::mat3x3 &value);
    void setMatrix(const std::string &propertyName, const glm::mat4x4& value);

    void setDoubleSided(bool bDoubleSided);
    bool getDoubleSided() { return m_DoubleSided; }

    void setAlphaMode(AlphaMode mode);
    AlphaMode getAlphaMode() { return m_AlphaMode; }

    void use();
private:
    Shader::Ptr m_Shader;

    std::map<std::string, Texture2D::Ptr> m_Textures;
    std::map<std::string, glm::vec3> m_UniformVec3;
    std::map<std::string, glm::vec4> m_UniformVec4;
    std::map<std::string, float> m_UniformFloats;

    bool m_DoubleSided;
    AlphaMode m_AlphaMode;
};