#include "base/ShaderProgram.h"

ShaderProgram::ShaderProgram(const std::string &name, const std::vector<Shader> shaders)
    : m_ShaderProgramName(name), m_Shaders(shaders)
{
    link();
}

ShaderProgram::~ShaderProgram()
{
    if (m_ShaderProgramID != 0)
        glDeleteProgram(m_ShaderProgramID);
}

void ShaderProgram::setUniformi(const std::string &uniformName, const int value)
{
    glUniform1i(getUniformLocation(uniformName), value);
}

void ShaderProgram::setUniform(const std::string &uniformName, const glm::mat4x4 &value)
{
    glUniformMatrix4fv(getUniformLocation(uniformName), 1, GL_FALSE, value_ptr(value));
}

void ShaderProgram::link()
{
    m_ShaderProgramID = glCreateProgram();

    for (size_t i = 0; i < m_Shaders.size(); ++i)
    {
        Shader shader = m_Shaders[i];
        glAttachShader(m_ShaderProgramID, shader.getShaderID());
    }
    
    glLinkProgram(m_ShaderProgramID);

    GLint success;
    glGetProgramiv(m_ShaderProgramID, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint logLength = 0;
        glGetProgramiv(m_ShaderProgramID, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> infoLog(logLength);
        glGetProgramInfoLog(m_ShaderProgramID, logLength, NULL, infoLog.data());
        std::cerr << "Failed to link shader program: " << m_ShaderProgramName << ". Error log:\n" << infoLog.data() << std::endl;
    }

    for (size_t i = 0; i < m_Shaders.size(); ++i)
    {
        Shader shader = m_Shaders[i];
        shader.deleteShader();
    }
}

GLuint ShaderProgram::getUniformLocation(const std::string &uniformName)
{
    return glGetUniformLocation(m_ShaderProgramID, uniformName.c_str());
}