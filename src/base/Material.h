#pragma once

#include <vector>

#include "ptr.h"
#include "base/Shader.h"
#include "base/Texture.h"

class Material
{
    SHARED_PTR(Material)
public:
    Material(Shader::Ptr shader);
    ~Material() = default;

    void addTexture(Texture::Ptr texture);

    void setInt(const std::string& propertyName, const int value);
    void setMatrix(const std::string& propertyName, const glm::mat4x4& value);

    void use();
private:
    Shader::Ptr m_Shader;
    std::vector<Texture::Ptr> m_Textures;
};