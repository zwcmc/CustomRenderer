#pragma once

#include <iostream>
#include <string>
#include <glad/glad.h>

class Texture
{
public:
    Texture();
    virtual ~Texture();

    virtual void bind();
    virtual void unbind();

    void initTexture(const std::string &textureName, int width, int height, int components, GLenum format, void* data, bool useMipmap = false);
    
    std::string& getTextureName() { return m_TextureName; }
    GLint getTextureUnitIndex() { return m_TextureUnitIndex; }

    static GLuint unitCount;

protected:
    std::string m_TextureName;
    GLuint m_TextureID;
    GLint m_TextureUnit;
    GLint m_TextureUnitIndex;
    int m_Width, m_Height, m_Components;
};