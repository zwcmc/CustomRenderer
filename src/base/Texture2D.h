#pragma once

#include <ktx.h>

#include "ptr.h"
#include "base/Texture.h"

class Texture2D : public Texture
{
    SHARED_PTR(Texture2D)
public:
    Texture2D(const std::string &name);
    ~Texture2D() = default;

    void initTexture2D(unsigned int width, unsigned int height, GLenum internalFormat, GLenum format, GLenum type, void* data, bool useMipmap = true);
    void initTexture2D(ktxTexture* kTexture, bool useMipmap = true);

    void setWrapMode(GLenum wrapS, GLenum wrapT);

private:
    GLenum m_WarpS, m_WarpT;
};