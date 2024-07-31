#pragma once

#include <iostream>
#include <string>

#include "base/Shader.h"
#include "base/Texture.h"

#include "tiny_gltf.h"

class AssetsLoader
{
public:
    static Shader::Ptr loadShader(const std::string &name, const std::string &vsFilePath, const std::string &fsFilePath);
    static Texture::Ptr loadTexture(const std::string &textureName, const std::string &filePath, bool flipVertical = true, bool useMipmap = false);
    static void loadglTFFile(const std::string &filePath);
private:
    AssetsLoader() = default;
    static std::string readShader(std::ifstream& file, const std::string& name);

    static void loadglTFNode(const tinygltf::Node &inputNode, const tinygltf::Model &input);

    inline static std::string getAssetsPath() { return "./../assets/"; }
    inline static std::string getShaderPath() { return "./../assets/glsl_shaders/"; }
};