#include "base/Shader.h"

#include <vector>
#include <fstream>

Shader::Shader()
    : m_ShaderID(0), m_ShaderName("")
{ }

Shader::Shader(const std::string &name, const std::string &vsSource, const std::string &fsSource)
    : m_ShaderID(0)
{
    m_ShaderName = name;
    createShadersAndCompile(vsSource, fsSource);
}

Shader::~Shader()
{
    if (m_ShaderID != 0)
        glDeleteProgram(m_ShaderID);
}

void Shader::use()
{
    if (m_ShaderID != 0)
        glUseProgram(m_ShaderID);
}

void Shader::setUniformInt(const std::string &uniformName, const int value)
{
    glUniform1i(getUniformLocation(uniformName), value);
}

void Shader::setUniformMatrix(const std::string &uniformName, const glm::mat4x4 &value)
{
    glUniformMatrix4fv(getUniformLocation(uniformName), 1, GL_FALSE, value_ptr(value));
}

GLuint Shader::getUniformLocation(const std::string &uniformName)
{
    return glGetUniformLocation(m_ShaderID, uniformName.c_str());
}

void Shader::createShadersAndCompile(const std::string &vsSource, const std::string &fsSource)
{
    GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fsID = glCreateShader(GL_FRAGMENT_SHADER);
    m_ShaderID = glCreateProgram();

    const GLchar* vsCode = vsSource.c_str();
    const GLchar* fsCode = fsSource.c_str();
    glShaderSource(vsID, 1, &vsCode, nullptr);
    glShaderSource(fsID, 1, &fsCode, nullptr);

    glCompileShader(vsID);
    glCompileShader(fsID);

    GLint success;
    GLint logLength = 0;
    std::vector<GLchar> infoLog;
    glGetShaderiv(vsID, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderiv(vsID, GL_INFO_LOG_LENGTH, &logLength);
        infoLog.resize(logLength);
        glGetShaderInfoLog(m_ShaderID, logLength, nullptr, infoLog.data());
        std::cerr << "Failed to compile VERTEX SHADER at: " << m_ShaderName << "! \n Error log: " << infoLog.data() << std::endl;
    }
    glGetShaderiv(fsID, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderiv(vsID, GL_INFO_LOG_LENGTH, &logLength);
        infoLog.resize(logLength);
        glGetShaderInfoLog(fsID, logLength, nullptr, infoLog.data());
        std::cerr << "Failed to compile FRAGMENT SHADER at: " << m_ShaderName << "! \n Error log: " << infoLog.data() << std::endl;
    }

    glAttachShader(m_ShaderID, vsID);
    glAttachShader(m_ShaderID, fsID);
    glLinkProgram(m_ShaderID);

    glGetProgramiv(m_ShaderID, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint logLength = 0;
        glGetProgramiv(m_ShaderID, GL_INFO_LOG_LENGTH, &logLength);
        infoLog.resize(logLength);
        glGetProgramInfoLog(m_ShaderID, logLength, nullptr, infoLog.data());
        std::cerr << "Failed to link SHADER PROGRAM at: " << m_ShaderName << "! Error log:\n" << infoLog.data() << std::endl;
    }

    glDeleteShader(vsID);
    glDeleteShader(fsID);
}