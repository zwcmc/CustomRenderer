#include "base/Shader.h"

#include <vector>

Shader::Shader(const std::string &name, const std::string &vsSource, const std::string &fsSource)
    : m_ShaderID(0)
{
    m_ShaderName = name;
    CreateShadersAndCompile(vsSource, fsSource);
}

Shader::~Shader()
{
    glDeleteProgram(m_ShaderID);
    m_ShaderID = 0;
}

void Shader::Use()
{
    glUseProgram(m_ShaderID);
}

void Shader::SetUniformInt(const std::string &uniformName, const int &value)
{
    glUniform1i(GetUniformLocation(uniformName), value);
}

void Shader::SetUniformFloat(const std::string &uniformName, const float &value)
{
    glUniform1f(GetUniformLocation(uniformName), value);
}

void Shader::SetUniformVector(const std::string &uniformName, const glm::vec3 &value)
{
    glUniform3fv(GetUniformLocation(uniformName), 1, &value[0]);
}

void Shader::SetUniformVector(const std::string &uniformName, const glm::vec4 &value)
{
    glUniform4fv(GetUniformLocation(uniformName), 1, &value[0]);
}

void Shader::SetUniformMatrix(const std::string &uniformName, const glm::mat3x3 &value)
{
    glUniformMatrix3fv(GetUniformLocation(uniformName), 1, GL_FALSE, &(value[0].x));
}

void Shader::SetUniformMatrix(const std::string &uniformName, const glm::mat4x4 &value)
{
    glUniformMatrix4fv(GetUniformLocation(uniformName), 1, GL_FALSE, &(value[0].x));
}

void Shader::SetUniformVectorArray(const std::string &uniformName, size_t size, const std::vector<glm::vec4> &values)
{
    glUniform4fv(GetUniformLocation(uniformName), static_cast<GLsizei>(size), (float*)(&values[0].x));
}

GLuint Shader::GetUniformLocation(const std::string &uniformName)
{
    return glGetUniformLocation(m_ShaderID, uniformName.c_str());
}

std::string& Shader::GetName()
{
    return m_ShaderName;
}

void Shader::CreateShadersAndCompile(const std::string &vsSource, const std::string &fsSource)
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
        glGetShaderInfoLog(vsID, logLength, nullptr, infoLog.data());
        std::cerr << "Failed to compile VERTEX SHADER at: " << m_ShaderName << "! \n Error log: " << infoLog.data() << std::endl;
    }
    glGetShaderiv(fsID, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderiv(fsID, GL_INFO_LOG_LENGTH, &logLength);
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

    // Set global uniform to binding point 0 for each shader
    GLuint uniformBlockIndex = glGetUniformBlockIndex(m_ShaderID, "GlobalUniforms");
    if (uniformBlockIndex != GL_INVALID_INDEX)
    {
        glUniformBlockBinding(m_ShaderID, uniformBlockIndex, 0);
    }
}
