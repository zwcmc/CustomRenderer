#pragma once

#include <ktx.h>

#include "ptr.h"
#include "base/Texture.h"

class Texture2D : public Texture
{
    SHARED_PTR(Texture2D)
public:
    Texture2D() = default;
    ~Texture2D() = default;

    void initTexture2D(const std::string& textureName, int width, int height, int components, GLenum format, void* data, bool useMipmap = true);
    void initTexture2D(const std::string& textureName, ktxTexture* kTexture, bool useMipmap = true);
};