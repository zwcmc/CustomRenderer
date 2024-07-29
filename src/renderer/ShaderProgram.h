#pragma once

#include <string>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class ShaderProgram
{
public:
    ShaderProgram(const std::string& name, const std::vector<Shader> shaders);
    ~ShaderProgram();

    void setUniform(const std::string& uniformName, const glm::mat4x4& value);

    inline void useProgram() { glUseProgram(m_ShaderProgramID); }
private:
    void link();

    GLuint getUniformLocation(const std::string& uniformName);

    std::string m_ShaderProgramName;
    GLuint m_ShaderProgramID;
    std::vector<Shader> m_Shaders;
};