#pragma once

#include <iostream>
#include <string>
#include <glad/glad.h>

class Texture
{
public:
    Texture();
    Texture(const std::string &textureName, const std::string &filePath, bool flipVertical = true, bool useMipmap = true);
    virtual ~Texture();

    virtual void bind();
    virtual void unbind();
    
    std::string& getTextureName() { return m_TextureName; }
    GLint getTextureUnitIndex() { return m_TextureUnitIndex; }

    static GLuint unitCount;

protected:
    std::string m_TextureName;
    GLuint m_TextureID;
    GLint m_TextureUnit;
    GLint m_TextureUnitIndex;

    GLint m_Width, m_Height, m_Components;
    bool m_UseMipmap;
    bool m_FlipVertical;
    std::string m_FilePath;

private:
    void generateTextureFromFile(const std::string &filePath);
    void initTexture(unsigned char* buffer);
};