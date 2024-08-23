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

    void initTexture2D(const glm::u32vec2 &size, GLenum internalFormat, GLenum format, GLenum type, void* data, bool useMipmap = false);
    void initTexture2D(ktxTexture* kTexture, bool useMipmap = false);
    void initShadowmap(const glm::u32vec2 &size);

    void setFilterMode(GLenum minFilter, GLenum magFilter);
    void setWrapMode(GLenum wrapS, GLenum wrapT);
    void resize(const glm::u32vec2 &size);
};
