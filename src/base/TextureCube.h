#pragma once

#include <ktx.h>

#include "ptr.h"
#include "base/Texture.h"

class TextureCube : public Texture
{
    SHARED_PTR(TextureCube)
public:
    TextureCube() = default;
    ~TextureCube() = default;

    void initTextureCube();
};