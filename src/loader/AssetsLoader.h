#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <assimp/scene.h>

#include "base/Shader.h"
#include "base/Texture2D.h"
#include "base/TextureCube.h"

#include "scene/SceneNode.h"

class AssetsLoader
{
public:
    static Shader::Ptr LoadShader(const std::string &name, const std::string &vsFilePath, const std::string &fsFilePath);
    static Texture2D::Ptr LoadTexture(const std::string &textureName, const std::string &filePath, bool useMipmap = false);
    static Texture2D::Ptr LoadHDRTexture(const std::string &textureName, const std::string &filePath, bool useMipmap = false);
    static SceneNode::Ptr LoadModel(const std::string &filePath, const bool &calculateAABB = true);

private:
    static std::string ReadShader(std::ifstream &file, const std::string &name);
    static SceneNode::Ptr ProcessAssimpNode(aiNode* aNode, const aiScene* aScene, const std::string &directory, const bool &calculateAABB);
    static Mesh::Ptr ParseMesh(aiMesh* aMesh, const aiScene* aScene);
    
    // Encode the orthonormal basis as a quaternion to save space in the attributes, and the sign of the w component preserve the reflection ((n x t) . b <= 0)
    static void EncodeTBN(const glm::vec3 &tangent, const glm::vec3 &bitangent, const glm::vec3 &normal, glm::quat &q);

    static Material::Ptr ParseMaterial(aiMaterial* aMaterial, const aiScene* aScene, const std::string& directory);
    static Texture2D::Ptr LoadAssimpTexture(const std::string &textureName, const std::string &directory, const std::string &texturePath);

    inline static std::string GetAssetsPath()
    {
#ifdef XCODE_PROJECT
        return "./../../assets/";
#else
        return "./../assets/";
#endif
    }

    inline static std::string GetShaderPath() { return GetAssetsPath() + "shaders/"; }
    inline static GLenum GetFormat(const int &components)
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

    static std::map<std::string, Texture2D::Ptr> assimpTextures;
};
