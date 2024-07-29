#include "opengl/Shader.h"

#include <vector>

#include "tool/AssetsLoader.h"

Shader::Shader(const std::string &code, const ShaderType &shaderType,  const std::string &filePath)
    : m_Code(code), m_ShaderType(shaderType), m_FilePath(filePath), m_ShaderID(0)
{
    // Load included files in shader
    scanCodeForIncludes();
    // Compile shader
    compileShader();
}

// Shader::Shader() : m_Code(""), m_ShaderType(ShaderType::NONE), m_FilePath(""), m_ShaderID(0) { }

void Shader::deleteShader()
{
    if (m_ShaderID != 0)
        glDeleteShader(m_ShaderID);
}

void Shader::scanCodeForIncludes()
{
    std::size_t startPos = 0;
    const static std::string includeDirective = "#include ";

    while ((startPos = m_Code.find(includeDirective, startPos)) != std::string::npos)
    {
        const std::size_t pos = startPos + includeDirective.length() + 1;
        const std::size_t length = m_Code.find('"', pos);
        const std::string pathToIncludedFile = m_Code.substr(pos, length - pos);

        const std::string includedFileCode = AssetsLoader::getInstance().loadCodeFromFile(pathToIncludedFile) + "\n";

        m_Code.replace(startPos, (length + 1) - startPos, includedFileCode);

        startPos += includedFileCode.length();
    }
}

void Shader::compileShader()
{
    switch (m_ShaderType)
    {
        case ShaderType::VERTEX:
            m_ShaderID = glCreateShader(GL_VERTEX_SHADER);
            break;
        case ShaderType::FRAGMENT:
            m_ShaderID = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        case ShaderType::GEOMETRY:
            m_ShaderID = glCreateShader(GL_GEOMETRY_SHADER);
        default:
            break;
    }

    const GLchar* code = m_Code.c_str();
    glShaderSource(m_ShaderID, 1, &code, NULL);
    glCompileShader(m_ShaderID);

    GLint success;
    glGetShaderiv(m_ShaderID, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint logLength = 0;
        glGetShaderiv(m_ShaderID, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> infoLog(logLength);
        glGetShaderInfoLog(m_ShaderID, logLength, NULL, infoLog.data());
        std::cerr << "Failed to compile shader. Shader path: " << m_FilePath << ". \n Error log: " << infoLog.data() << std::endl;
    }
}

Shader Shader::fromFile(const std::string &filePath, const ShaderType &shaderType)
{
    const std::string code = AssetsLoader::getInstance().loadCodeFromFile(filePath);
    return Shader(code, shaderType, filePath);
}