#pragma once

#include <iostream>
#include <string>

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ptr.h"

class Shader
{
    SHARED_PTR(Shader)
public:
    Shader();
    Shader(const std::string &name, const std::string &vsSource, const std::string &fsSource);
    ~Shader();

    void use();

    void setUniformInt(const std::string &uniformName, const int value);
    void setUniformFloat(const std::string& uniformName, const float &value);
    void setUniformVector(const std::string & uniformName, const glm::vec4 &value);
    void setUniformMatrix(const std::string &uniformName, const glm::mat4x4 &value);

    std::string getShaderName() { return m_ShaderName; }

    GLuint m_ShaderID;
private:
    void createShadersAndCompile(const std::string &vsSource, const std::string &fsSource);

    GLuint getUniformLocation(const std::string &uniformName);

    std::string m_ShaderName;
};