#pragma once

#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

#include "ptr.h"

class Texture
{
    SHARED_PTR(Texture)
public:
    Texture();
    ~Texture();

    void setTextureName(const std::string &name) { m_TextureName = name; }
    std::string& getTextureName() { return m_TextureName; }
    GLuint& getTextureID() { return m_TextureID; }
    glm::u32vec2& getSize() { return m_Size; }

    void bind(int unit = -1);
    void unbind();

protected:
    std::string m_TextureName;
    GLuint m_TextureID;
    GLenum m_InternalFormat, m_Format, m_Type, m_Target;
    glm::u32vec2 m_Size;
};