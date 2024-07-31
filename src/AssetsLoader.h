#pragma once

#include <iostream>
#include <string>

#include "base/Shader.h"
#include "base/Texture.h"

class AssetsLoader
{
public:
    static Shader* loadShader(const std::string &name, const std::string &vsFilePath, const std::string &fsFilePath);
    static Texture* loadTexture(const std::string& textureName, const std::string& filePath, bool flipVertical = true, bool useMipmap = false);
private:
    AssetsLoader() = default;
    static std::string readShader(std::ifstream& file, const std::string& name);

    inline static std::string getAssetsPath() { return "./../assets/"; }
    inline static std::string getShaderPath() { return "./../assets/glsl_shaders/"; }
};