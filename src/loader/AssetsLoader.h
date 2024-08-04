#pragma once

#include <iostream>
#include <string>

#include "base/Shader.h"
#include "base/Texture.h"

#include "tiny_gltf.h"

#include "renderer/glTFRenderer.h"

class AssetsLoader
{
public:
    static Shader::Ptr loadShaderFromFile(const std::string &name, const std::string &vsFilePath, const std::string &fsFilePath);
    static Texture::Ptr loadTextureFromFile(const std::string &textureName, const std::string &filePath, bool useMipmap = false);
    static Texture::Ptr createTextureFromBuffer(const std::string &textureName, const int &width, const int &height, const int &components, void* buffer, bool useMipmap = false);
    static glTFRenderer::Ptr loadglTFFile(const std::string &filePath);

private:
    AssetsLoader() = default;

    static std::string readShader(std::ifstream& file, const std::string& name);
    static void loadglTFMaterials(const tinygltf::Model &input);
    static void loadglTFNode(const tinygltf::Node &inputNode, const tinygltf::Model &input, glTFRenderer::Ptr renderer, glTFRenderer::glTFNode::Ptr parent);

    inline static std::string getAssetsPath() { return "./../assets/"; }
    inline static std::string getShaderPath() { return "./../assets/glsl_shaders/"; }
    inline static GLenum getFormat(const int &components)
    {
        GLenum format = 0;
        switch (components)
        {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        }
        return format;
    }

    static std::vector<glTFRenderer::glTFMaterialData::Ptr> glTFMatDatas;
};