#pragma once

#include <iostream>
#include <string>
#include <glad/glad.h>

#include "ptr.h"

class Texture
{
SHARED_PTR(Texture)
public:
    Texture();
    virtual ~Texture();

    void initTexture(const std::string &textureName, int width, int height, int components, GLenum format, void* data, bool useMipmap = false);
    
    void setTextureName(const std::string &name) { m_TextureName = name; }
    std::string& getTextureName() { return m_TextureName; }
    GLuint getTextureID() { return m_TextureID; }

protected:
    std::string m_TextureName;
    GLuint m_TextureID;
    int m_Width, m_Height, m_Components;
};