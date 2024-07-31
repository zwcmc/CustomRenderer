#pragma once

#include <iostream>
#include <string>

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    Shader(const std::string &name, const std::string &vsSource, const std::string &fsSource);
    ~Shader();

    void use();

    void setUniformi(const std::string &uniformName, const int value);
    void setUniform(const std::string &uniformName, const glm::mat4x4 &value);

    static Shader* fromFile(const std::string &name, const std::string &vsPath, const std::string &fsPath);
private:
    static std::string readCodeFromFile(const std::string &filePath);
    void createShadersAndCompile(const std::string &vsSource, const std::string &fsSource);

    GLuint getUniformLocation(const std::string &uniformName);

    std::string m_ShaderName;
    GLuint m_ShaderID;
};