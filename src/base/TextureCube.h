#pragma once

#include <ktx.h>

#include "ptr.h"
#include "base/Texture.h"

class TextureCube : public Texture
{
    SHARED_PTR(TextureCube)
public:
    TextureCube(const std::string &name);
    ~TextureCube() = default;

    void InitTextureCube(ktxTexture* kTexture, bool useMipmap = false);
    void DefaultInit(const unsigned int &width, const unsigned int &height, GLenum internalFormat, GLenum format, GLenum type, bool useMipmap = false);
};