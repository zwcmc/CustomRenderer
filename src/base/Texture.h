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

    void SetTextureName(const std::string &name) { m_TextureName = name; }
    std::string& GetTextureName() { return m_TextureName; }
    GLuint& GetTextureID() { return m_TextureID; }
    glm::u32vec2& GetSize() { return m_Size; }

    void Bind(const int &unit = -1);
    void Unbind();

protected:
    std::string m_TextureName;
    GLuint m_TextureID;
    GLenum m_InternalFormat, m_Format, m_Type, m_Target;
    glm::u32vec2 m_Size;
};