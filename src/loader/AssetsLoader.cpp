#include "loader/AssetsLoader.h"

#include <fstream>
#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>

#include <ktx.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/GltfMaterial.h>

#include "base/Material.h"

#include "utility/Collision.h"

using namespace Collision;

std::map<std::string, Texture2D::Ptr> AssetsLoader::assimpTextures = {};

Shader::Ptr AssetsLoader::loadShader(const std::string &name, const std::string &vsFilePath, const std::string &fsFilePath)
{
    std::string vsPath = getAssetsPath() + vsFilePath;
    std::string fsPath = getAssetsPath() + fsFilePath;

    std::ifstream vsFile, fsFile;
    vsFile.open(vsPath);
    fsFile.open(fsPath);

    if (!vsFile.is_open() || !fsFile.is_open())
    {
        std::cerr << "Failed to load shader, path: " + vsPath + " and " + fsPath << std::endl;
        return Shader::New();
    }

    std::string vsSource = readShader(vsFile, name);
    std::string fsSource = readShader(fsFile, name);

    vsFile.close();
    fsFile.close();

    return Shader::New(name, vsSource, fsSource);
}

Texture2D::Ptr AssetsLoader::loadTexture(const std::string &textureName, const std::string &filePath, bool useMipmap)
{
    Texture2D::Ptr texture = Texture2D::New(textureName);

    stbi_set_flip_vertically_on_load(true);

    std::string newPath = getAssetsPath() + filePath;
    int width, height, components;
    unsigned char* data = stbi_load(newPath.c_str(), &width, &height, &components, 0);
    if (data)
    {
        GLenum format = getFormat(components);
        texture->initTexture2D(glm::u32vec2(width, height), format, format, GL_UNSIGNED_BYTE, data, useMipmap);
    }
    else
    {
        std::cerr << "Failed to load texture: " << newPath << std::endl;
    }

    stbi_image_free(data);

    stbi_set_flip_vertically_on_load(false);

    return texture;
}

Texture2D::Ptr AssetsLoader::loadHDRTexture(const std::string &textureName, const std::string &filePath, bool useMipmap)
{
    Texture2D::Ptr texture = Texture2D::New(textureName);

    stbi_set_flip_vertically_on_load(true);

    std::string newPath = getAssetsPath() + filePath;
    if (stbi_is_hdr(newPath.c_str()))
    {
        int width, height, components;
        float* data = stbi_loadf(newPath.c_str(), &width, &height, &components, 0);
        if (data)
        {
            GLenum internalFormat, format = 0;
            if (components == 3)
            {
                internalFormat = GL_RGB32F;
                format = GL_RGB;
            }
            else if (components == 4)
            {
                internalFormat = GL_RGBA32F;
                format = GL_RGBA;
            }

            texture->initTexture2D(glm::u32vec2(width, height), internalFormat, format, GL_FLOAT, data, useMipmap);
            texture->setWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        }
        stbi_image_free(data);
    }
    else
    {
        std::cerr << newPath << " is not a HDR file!" << std::endl;
    }

    stbi_set_flip_vertically_on_load(false);

    return texture;
}

Texture2D::Ptr AssetsLoader::loadTextureBuffer(const std::string &textureName, const glm::u32vec2 &size, const int &components, GLenum type, void* buffer, bool useMipmap)
{
    Texture2D::Ptr texture = Texture2D::New(textureName);

    if (buffer)
    {
        GLenum format = getFormat(components);
        texture->initTexture2D(size, format, format, type, buffer, useMipmap);
    }
    else
    {
        std::cerr << "Failer to create texture: "<< textureName << "! Texture2D buffer is nullptr." << std::endl;
    }

    return texture;
}

Texture2D::Ptr AssetsLoader::loadTextureKTX(const std::string &textureName, const std::string &filePath, bool useMipmap)
{
    ktxTexture* kTexture;
    KTX_error_code result;

    std::string newPath = getAssetsPath() + filePath;
    result = ktxTexture_CreateFromNamedFile(newPath.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &kTexture);

    assert(result == KTX_SUCCESS);

    Texture2D::Ptr texture = Texture2D::New(textureName);
    texture->initTexture2D(kTexture, useMipmap);
    ktxTexture_Destroy(kTexture);
    return texture;
}

TextureCube::Ptr AssetsLoader::loadCubemapKTX(const std::string &textureName, const std::string &filePath)
{
    ktxTexture* kTexture;
    KTX_error_code result;

    std::string newPath = getAssetsPath() + filePath;
    result = ktxTexture_CreateFromNamedFile(newPath.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &kTexture);

    assert(result == KTX_SUCCESS);

    // must be a cubemap texture
    assert(kTexture->numFaces == 6);

    TextureCube::Ptr textureCube = TextureCube::New(textureName);
    textureCube->initTextureCube(kTexture);

    ktxTexture_Destroy(kTexture);

    return textureCube;
}

void AssetsLoader::initCubemapKTX(TextureCube::Ptr cubemap, const std::string &filePath)
{
    ktxTexture *kTexture;
    KTX_error_code result;

    std::string newPath = getAssetsPath() + filePath;
    result = ktxTexture_CreateFromNamedFile(newPath.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &kTexture);

    assert(result == KTX_SUCCESS);

    // must be a cubemap texture
    assert(kTexture->numFaces == 6);

    cubemap->initTextureCube(kTexture);

    ktxTexture_Destroy(kTexture);
}

std::string AssetsLoader::readShader(std::ifstream &file, const std::string &name)
{
    std::string source, line;
    const std::string includeDirective = "#include ";
    const size_t startPos = includeDirective.length();
    while (std::getline(file, line))
    {
        if (line.substr(0, startPos) == includeDirective)
        {
            const size_t pos = startPos + 1;
            std::string includedPath = getShaderPath() + line.substr(pos, line.length() - pos - 1);
            std::ifstream includedFile(includedPath);
            if (includedFile.is_open())
            {
                source += readShader(includedFile, name);
            }
            else
            {
                std::cerr << "Failed to read included file: " << includedPath << " in Shader: " << name << std::endl;
            }
            includedFile.close();
        }
        else
        {
            source += line + "\n";
        }
    }
    return source;
}

SceneNode::Ptr AssetsLoader::loadModel(const std::string &filePath, const bool &ingoreAABBCalculation)
{
    std::string newPath = getAssetsPath() + filePath;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(newPath, aiProcess_Triangulate);

    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Assimp: load file error, error message: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    std::string directory = filePath.substr(0, filePath.find_last_of("/"));


    AssetsLoader::assimpTextures.clear();
    return AssetsLoader::processAssimpNode(scene->mRootNode, scene, directory, ingoreAABBCalculation);
}

SceneNode::Ptr AssetsLoader::processAssimpNode(aiNode* aNode, const aiScene* aScene, const std::string& directory, const bool &ingoreAABBCalculation)
{
    SceneNode::Ptr node = SceneNode::New();
    node->ModelMatrix = AssetsLoader::aiMatrix4x4ToGlmMat4(aNode->mTransformation);

    for (size_t i = 0; i < aNode->mNumMeshes; ++i)
    {
        aiMesh* assimpMesh = aScene->mMeshes[aNode->mMeshes[i]];
        aiMaterial* assimpMat = aScene->mMaterials[assimpMesh->mMaterialIndex];

        glm::vec3 aabbMin = glm::vec3(FLT_MAX);
        glm::vec3 aabbMax = glm::vec3(-FLT_MAX);

        Mesh::Ptr mesh = AssetsLoader::parseMesh(assimpMesh, aScene, aabbMin, aabbMax, ingoreAABBCalculation);
        Material::Ptr mat = AssetsLoader::parseMaterial(assimpMat, aScene, directory);
        
        if (!ingoreAABBCalculation)
        {
            if (!node->IsAABBCalculated && assimpMesh->mNumVertices > 0)
                node->IsAABBCalculated = true;

            BoundingBox::CreateFromPoints(node->AABB, aabbMin, aabbMax);
        }

        node->MeshRenders.push_back(MeshRender::New(mesh, mat));
    }

    // also recursively parse this node's children
    for (unsigned int i = 0; i < aNode->mNumChildren; ++i)
        node->addChild(AssetsLoader::processAssimpNode(aNode->mChildren[i], aScene, directory, ingoreAABBCalculation));

    return node;
}

Mesh::Ptr AssetsLoader::parseMesh(aiMesh* aMesh, const aiScene* aScene, glm::vec3 &outAABBMin, glm::vec3 &outAABBMax, const bool &ingoreAABBCalculation)
{
    // Vertices
    std::vector<vec3> vertices;
    std::vector<vec2> texcoords;
    std::vector<vec3> normals;
    std::vector<unsigned int> indices;
    
    vertices.resize(aMesh->mNumVertices);
    texcoords.resize(aMesh->mNumVertices);
    normals.resize(aMesh->mNumVertices);
    
    // Assume a constant of 3 vertex indices per face as always "aiProcess_Triangulate" in Assimp's post-processing step
    indices.resize(aMesh->mNumFaces * 3);

    for (size_t i = 0; i < aMesh->mNumVertices; ++i)
    {
        vertices[i] = glm::vec3(aMesh->mVertices[i].x, aMesh->mVertices[i].y, aMesh->mVertices[i].z);
        texcoords[i] = aMesh->mTextureCoords[0] ? glm::vec2(aMesh->mTextureCoords[0][i].x, aMesh->mTextureCoords[0][i].y) : glm::vec2(0.0f);
        normals[i] = glm::vec3(aMesh->mNormals[i].x, aMesh->mNormals[i].y, aMesh->mNormals[i].z);

        // Calculate AABB
        if (!ingoreAABBCalculation)
        {
            outAABBMin = glm::min(outAABBMin, vertices[i]);
            outAABBMax = glm::max(outAABBMax, vertices[i]);
//            if (vertices[i].x < outAABBMin.x) outAABBMin.x = vertices[i].x;
//            if (vertices[i].y < outAABBMin.y) outAABBMin.y = vertices[i].y;
//            if (vertices[i].z < outAABBMin.z) outAABBMin.z = vertices[i].z;
//            if (vertices[i].x > outAABBMax.x) outAABBMax.x = vertices[i].x;
//            if (vertices[i].y > outAABBMax.y) outAABBMax.y = vertices[i].y;
//            if (vertices[i].z > outAABBMax.z) outAABBMax.z = vertices[i].z;
        }
    }

    for (size_t f = 0; f < aMesh->mNumFaces; ++f)
    {
        aiFace face = aMesh->mFaces[f];
        for (size_t i = 0; i < 3; ++i)
            indices[3 * f + i] = face.mIndices[i];
    }

    return Mesh::New(vertices, texcoords, normals, indices);
}

Material::Ptr AssetsLoader::parseMaterial(aiMaterial* aMaterial, const aiScene* aScene, const std::string& directory)
{
    Material::Ptr mat = Material::New("Blinn-Phong", "glsl_shaders/BlinnPhong.vert", "glsl_shaders/BlinnPhong.frag");
    // Material::Ptr mat = Material::New("PBR", "glsl_shaders/PBRLit.vert", "glsl_shaders/PBRLit.frag");

    // Base map
    aiString texturePath;
    if (AI_SUCCESS == aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath))
    {
        mat->addOrSetTexture(AssetsLoader::loadAssimpTexture("uBaseMap", directory, texturePath.C_Str()));
        mat->addOrSetFloat("uBaseMapSet", 1.0f);
    }
    else
    {
        mat->addOrSetFloat("uBaseMapSet", -1.0f);
    }

    // Base color
    aiColor4D color;
    if (AI_SUCCESS == aiGetMaterialColor(aMaterial, AI_MATKEY_COLOR_DIFFUSE, &color))
        mat->addOrSetVector("uBaseColor", glm::vec4(color.r, color.g, color.b, color.a));
    else
        mat->addOrSetVector("uBaseColor", glm::vec4(1.0f));

    // Normal map
    if (AI_SUCCESS == aMaterial->GetTexture(aiTextureType_NORMALS, 0, &texturePath))
    {
        mat->addOrSetTexture(AssetsLoader::loadAssimpTexture("uNormalMap", directory, texturePath.C_Str()));
        mat->addOrSetFloat("uNormalMapSet", 1.0f);
    }
    else
        mat->addOrSetFloat("uNormalMapSet", -1.0f);

    // Emission
    if (AI_SUCCESS == aMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath))
    {
        mat->addOrSetTexture(AssetsLoader::loadAssimpTexture("uEmissiveMap", directory, texturePath.C_Str()));
        mat->addOrSetFloat("uEmissiveMapSet", 1.0f);
    }
    else
        mat->addOrSetFloat("uEmissiveMapSet", -1.0f);
    
    // Emission color
    if (AI_SUCCESS == aiGetMaterialColor(aMaterial, AI_MATKEY_COLOR_EMISSIVE, &color))
        mat->addOrSetVector("uEmissiveColor", glm::vec3(color.r, color.g, color.b));
    else
        mat->addOrSetVector("uEmissiveColor", glm::vec3(1.0f));

    // Metallic roughness texture
    // aiTextureType_METALNESS or aiTextureType_DIFFUSE_ROUGHNESS
    if (AI_SUCCESS == aMaterial->GetTexture(aiTextureType_METALNESS, 0, &texturePath))
    {
        mat->addOrSetTexture(AssetsLoader::loadAssimpTexture("uMetallicRoughnessMap", directory, texturePath.C_Str()));
        mat->addOrSetFloat("uMetallicRoughnessMapSet", 1.0f);
    }
    else
        mat->addOrSetFloat("uMetallicRoughnessMapSet", -1.0f);
    
    // Metallic factor
    ai_real valueFactor;
    if (AI_SUCCESS == aiGetMaterialFloat(aMaterial, AI_MATKEY_METALLIC_FACTOR, &valueFactor))
        mat->addOrSetFloat("uMetallicFactor", valueFactor);
    else
        mat->addOrSetFloat("uMetallicFactor", 0.0f);
    
    // Roughness factor
    if (AI_SUCCESS == aiGetMaterialFloat(aMaterial, AI_MATKEY_ROUGHNESS_FACTOR, &valueFactor))
        mat->addOrSetFloat("uRoughnessFactor", valueFactor);
    else
        mat->addOrSetFloat("uRoughnessFactor", 0.8f);

    // Occlusion map
    if (AI_SUCCESS == aMaterial->GetTexture(aiTextureType_LIGHTMAP, 0, &texturePath))
    {
        mat->addOrSetTexture(AssetsLoader::loadAssimpTexture("uOcclusionMap", directory, texturePath.C_Str()));
        mat->addOrSetFloat("uOcclusionMapSet", 1.0f);
    }
    else
        mat->addOrSetFloat("uOcclusionMapSet", -1.0f);

    // Cull face
    int two_sided;
    if((AI_SUCCESS == aiGetMaterialInteger(aMaterial, AI_MATKEY_TWOSIDED, &two_sided)) && two_sided)
        mat->setDoubleSided(true);
    else
        mat->setDoubleSided(false);

    aiString alphaMode("OPAQUE");
    Material::AlphaMode mode = Material::AlphaMode::DEFAULT_OPAQUE;
    if (AI_SUCCESS == aiGetMaterialString(aMaterial, AI_MATKEY_GLTF_ALPHAMODE, &alphaMode))
    {
        std::string m = alphaMode.C_Str();
        if (m == "MASK")
            mode = Material::AlphaMode::MASK;
        else if (m == "BLEND")
            mode = Material::AlphaMode::BLEND;
    }
    mat->setAlphaMode(mode);
    mat->addOrSetFloat("uAlphaBlendSet", mode == Material::AlphaMode::BLEND ? 1.0f : -1.0f);
    mat->addOrSetFloat("uAlphaTestSet", mode == Material::AlphaMode::MASK ? 1.0f : -1.0f);
    
    // Alpha cuteoff
    if (AI_SUCCESS == aiGetMaterialFloat(aMaterial, AI_MATKEY_GLTF_ALPHACUTOFF, &valueFactor))
        mat->addOrSetFloat("uAlphaCutoff", valueFactor);
    else
        mat->addOrSetFloat("uAlphaCutoff", 0.5f);

    return mat;
}

Texture2D::Ptr AssetsLoader::loadAssimpTexture(const std::string &textureName, const std::string &directory, const std::string &texturePath)
{
    auto it = AssetsLoader::assimpTextures.find(texturePath);
    if (it != AssetsLoader::assimpTextures.end())
    {
        return it->second;
    }

    Texture2D::Ptr texture = AssetsLoader::loadTexture(textureName, directory + "/" + texturePath, true);
    AssetsLoader::assimpTextures.insert(std::make_pair(texturePath, texture));
    return texture;
}
