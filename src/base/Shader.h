#pragma once

#include <iostream>
#include <string>

#include <glad/glad.h>

class Shader
{
public:
    enum class ShaderType
    {
        NONE,
        VERTEX,
        FRAGMENT,
        GEOMETRY
    };

    Shader(const std::string &code, const ShaderType &shaderType, const std::string &filePath);
    // Shader();
    ~Shader() = default;

    inline GLuint getShaderID() const { return m_ShaderID; };
    static Shader readFromFile(const std::string &filePath, const ShaderType &shaderType);

    void deleteShader();
private:
    void scanCodeForIncludes();
    void compileShader();

    std::string m_Code;
    ShaderType m_ShaderType;
    std::string m_FilePath;
    GLuint m_ShaderID;
};