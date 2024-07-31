#pragma once

#include <vector>

#include "base/Shader.h"
#include "base/Texture.h"

class Material
{
public:
    Material(Shader* shader);
    ~Material() = default;

    void addTexture(Texture* texture);

    void setInt(const std::string& propertyName, const int value);
    void setMatrix(const std::string& propertyName, const glm::mat4x4& value);

    void use();
private:
    Shader* m_Shader;
    std::vector<Texture*> m_Textures;
};