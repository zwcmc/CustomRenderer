#pragma once

#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include "tiny_gltf.h"
#include <assimp/scene.h>

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

    static Shader::Ptr loadShader(const std::string &name, const std::string &vsFilePath, const std::string &fsFilePath);
    static Texture2D::Ptr loadTexture(const std::string &textureName, const std::string &filePath, bool useMipmap = false);
    static Texture2D::Ptr loadHDRTexture(const std::string &textureName, const std::string &filePath, bool useMipmap = false);
    static Texture2D::Ptr loadTextureBuffer(const std::string &textureName, const glm::u32vec2 &size, const int &components, GLenum type, void* buffer, bool useMipmap = false);
    static RenderNode::Ptr load_glTF(const std::string &filePath);
    static Texture2D::Ptr loadTextureKTX(const std::string &textureName, const std::string &filePath, bool useMipmap = false);
    static TextureCube::Ptr loadCubemapKTX(const std::string &textureName, const std::string &filePath);
    static void initCubemapKTX(TextureCube::Ptr cubemap, const std::string &filePath);

    static RenderNode::Ptr loadObj(const std::string &filePath);

private:
    AssetsLoader() = default;

    static void load_glTFMaterials(const tinygltf::Model &input, RenderNode::Ptr rootNode);
    static void load_glTFNode(const tinygltf::Node &inputNode, const tinygltf::Model &input, RenderNode::Ptr parent);
    static std::string readShader(std::ifstream &file, const std::string &name);

    static RenderNode::Ptr processAssimpNode(aiNode* aNode, const aiScene* aScene, const std::string &directory, RenderNode::Ptr parent);
    static Mesh::Ptr parseMesh(aiMesh* aMesh, const aiScene* aScene);
    static Material::Ptr parseMaterial(aiMaterial* aMaterial, const aiScene* aScene, const std::string& directory);

    inline static std::string getAssetsPath()
    {
#ifdef XCODE_PROJECT
        return "./../../assets/";
#else
        return "./../assets/";
#endif
    }

    inline static std::string getShaderPath() { return getAssetsPath() + "glsl_shaders/"; }
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
    static glm::mat4 aiMatrix4x4ToGlmMat4(const aiMatrix4x4 &from)
    {
        glm::mat4 to;
        
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

        return to;
    }

    static std::vector<glTFMaterialData::Ptr> glTFMatDatas;
};
