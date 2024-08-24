#pragma once

#include <iostream>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "ptr.h"

class Shader
{
    SHARED_PTR(Shader)
public:
    Shader();
    Shader(const std::string &name, const std::string &vsSource, const std::string &fsSource);
    ~Shader();

    void Use();

    void SetUniformInt(const std::string &uniformName, const int &value);
    void SetUniformFloat(const std::string &uniformName, const float &value);
    void SetUniformVector(const std::string &uniformName, const glm::vec3 &value);
    void SetUniformVector(const std::string &uniformName, const glm::vec4 &value);
    void SetUniformMatrix(const std::string &uniformName, const glm::mat3x3 &value);
    void SetUniformMatrix(const std::string &uniformName, const glm::mat4x4 &value);

private:
    void CreateShadersAndCompile(const std::string &vsSource, const std::string &fsSource);
    GLuint GetUniformLocation(const std::string &uniformName);

    GLuint m_ShaderID;
    std::string m_ShaderName;
};