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

    void InitTexture2D(const glm::u32vec2 &size, GLenum internalFormat, GLenum format, GLenum type, void* data, bool useMipmap = false);
    void InitTexture2D(ktxTexture* kTexture, bool useMipmap = false);
    void InitShadowMap(const glm::u32vec2 &size);

    void SetFilterMode(GLenum minFilter, GLenum magFilter);
    void SetWrapMode(GLenum wrapS, GLenum wrapT);
    void SetSize(const glm::u32vec2 &size);
};
