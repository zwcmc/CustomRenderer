#pragma once

#include <iostream>
#include <string>

#include "tiny_gltf.h"

#include "base/Shader.h"
#include "base/Texture2D.h"
#include "base/TextureCube.h"
#include "renderer/RenderNode.h"

class AssetsLoader
{
public:
    struct glTFMaterialData
    {
        SHARED_PTR_STRUCT(glTFMaterialData)

        Texture2D::Ptr baseColorTexture;
        glm::vec4 baseColorFactor;

        Texture2D::Ptr normalTexture;

        Texture2D::Ptr emissiveTexture;
        glm::vec3 emissiveFactor;

        Texture2D::Ptr metallicRoughnessTexture;
        float metallicFactor;
        float roughnessFactor;

        Texture2D::Ptr occlusionTexture;

        bool doubleSided;

        Material::AlphaMode alphaMode;
        float alphaCutoff;

        glTFMaterialData()
            : baseColorFactor(glm::vec4(1.0f)), emissiveFactor(glm::vec3(0.0f)), metallicFactor(1.0f), roughnessFactor(1.0f),
            doubleSided(false), alphaMode(Material::AlphaMode::DEFAULT_OPAQUE), alphaCutoff(1.0f)
        { }
    };

    static Shader::Ptr loadShaderFromFile(const std::string &name, const std::string &vsFilePath, const std::string &fsFilePath);
    static Texture2D::Ptr loadTexture2DFromFile(const std::string &textureName, const std::string &filePath, bool useMipmap = true);
    static Texture2D::Ptr createTexture2DFromBuffer(const std::string &textureName, const int &width, const int &height, const int &components, void* buffer, bool useMipmap = true);
    static RenderNode::Ptr loadglTFFile(const std::string &filePath);

    static Texture2D::Ptr loadTexture2DFromKTXFile(const std::string &textureName, const std::string &filePath, bool useMipmap = true);
    static TextureCube::Ptr loadCubemapFromKTXFile(const std::string& filePath);

private:
    AssetsLoader() = default;

    static void loadglTFMaterials(const tinygltf::Model &input, RenderNode::Ptr rootNode);
    static void loadglTFNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, RenderNode::Ptr parent);
    static std::string readShader(std::ifstream& file, const std::string& name);

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

    static std::vector<glTFMaterialData::Ptr> glTFMatDatas;
};