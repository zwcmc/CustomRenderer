#include "loader/AssetsLoader.h"

#include <fstream>
#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>

#include <ktx.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/GltfMaterial.h>

#include "base/Material.h"

std::vector<AssetsLoader::glTFMaterialData::Ptr> AssetsLoader::glTFMatDatas = {};
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

RenderNode::Ptr AssetsLoader::load_glTF(const std::string &filePath)
{
    bool binary = false;
    size_t extPos = filePath.rfind('.', filePath.length());
    if (extPos != std::string::npos)
    {
        binary = (filePath.substr(extPos + 1, filePath.length() - extPos) == "glb");
    }

    std::string newPath = getAssetsPath() + filePath;

    tinygltf::Model glTFInput;
    tinygltf::TinyGLTF glTFContext;
    std::string error, warning;

    RenderNode::Ptr rootNode = RenderNode::New();

    bool fileLoaded = binary ? glTFContext.LoadBinaryFromFile(&glTFInput, &error, &warning, newPath) : glTFContext.LoadASCIIFromFile(&glTFInput, &error, &warning, newPath);
    if (fileLoaded)
    {
        // Load texture and material data
        load_glTFMaterials(glTFInput, rootNode);

        const tinygltf::Scene &scene = glTFInput.scenes[0];
        for (size_t i = 0; i < scene.nodes.size(); ++i)
        {
            const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
            load_glTFNode(node, glTFInput, rootNode);
        }
    }
    else
    {
        std::cerr << "Could not open the glTF file: " << newPath << ", error: " << error << std::endl;
    }

    return rootNode;
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

void AssetsLoader::load_glTFMaterials(const tinygltf::Model &input, RenderNode::Ptr rootNode)
{
    // Load texture indices
    std::vector<int> textureIndices;
    {
        textureIndices.resize(input.textures.size());
        for (size_t index = 0; index < input.textures.size(); ++index)
        {
            textureIndices[index] = input.textures[index].source;
        }
    }

    // Create textures
    rootNode->NodeTextures.clear();
    {
        rootNode->NodeTextures.resize(input.images.size());
        for (size_t index = 0; index < input.images.size(); ++index)
        {
            const tinygltf::Image &glTFImage = input.images[index];
            unsigned char *buffer = const_cast<unsigned char *>(&glTFImage.image[0]);
            rootNode->NodeTextures[index] = loadTextureBuffer(glTFImage.name, glm::u32vec2(glTFImage.width, glTFImage.height), glTFImage.component,
                glTFImage.pixel_type, reinterpret_cast<void *>(buffer), true);
        }
    }

    // load glTF materials
    AssetsLoader::glTFMatDatas.clear();
    {
        AssetsLoader::glTFMatDatas.resize(input.materials.size());
        for (size_t index = 0; index < input.materials.size(); ++index)
        {
            AssetsLoader::glTFMatDatas[index] = glTFMaterialData::New();

            tinygltf::Material mat = input.materials[index];

            glTFMaterialData::Ptr matData = AssetsLoader::glTFMatDatas[index];

            // Base map
            if (mat.values.find("baseColorTexture") != mat.values.end())
            {
                int textureIndex = mat.values["baseColorTexture"].TextureIndex();
                matData->baseColorTexture = rootNode->NodeTextures[textureIndices[textureIndex]];
            }
            // Base color
            if (mat.values.find("baseColorFactor") != mat.values.end())
            {
                matData->baseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
            }
            // Nomral map
            if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end())
            {
                int normalTextureIndex = mat.additionalValues["normalTexture"].TextureIndex();
                matData->normalTexture = rootNode->NodeTextures[textureIndices[normalTextureIndex]];
            }

            // Emissive
            if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end())
            {
                int emissiveTextureIndex = mat.additionalValues["emissiveTexture"].TextureIndex();
                matData->emissiveTexture = rootNode->NodeTextures[textureIndices[emissiveTextureIndex]];
            }
            if (mat.additionalValues.find("emissiveFactor") != mat.additionalValues.end())
            {
                matData->emissiveFactor = glm::make_vec3(mat.additionalValues["emissiveFactor"].ColorFactor().data());
            }

            // Metallic roughness
            if (mat.values.find("metallicRoughnessTexture") != mat.values.end())
            {
                int metallicRoughnessTextureIndex = mat.values["metallicRoughnessTexture"].TextureIndex();
                matData->metallicRoughnessTexture = rootNode->NodeTextures[textureIndices[metallicRoughnessTextureIndex]];
            }
            if (mat.values.find("metallicFactor") != mat.values.end())
            {
                matData->metallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
            }
            if (mat.values.find("roughnessFactor") != mat.values.end())
            {
                matData->roughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
            }

            // Occlusion
            if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end())
            {
                int occlusionTextureIndex = mat.additionalValues["occlusionTexture"].TextureIndex();
                matData->occlusionTexture = rootNode->NodeTextures[textureIndices[occlusionTextureIndex]];
            }

            // Doublesided
            matData->doubleSided = mat.doubleSided;

            // Alpha blend and alpha test
            if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end())
            {
                tinygltf::Parameter param = mat.additionalValues["alphaMode"];
                if (param.string_value == "BLEND")
                {
                    matData->alphaMode = Material::AlphaMode::BLEND;
                }
                if (param.string_value == "MASK")
                {
                    matData->alphaMode = Material::AlphaMode::MASK;
                    matData->alphaCutoff = 0.5f;
                }
            }
            if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end())
            {
                matData->alphaCutoff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
            }
        }
    }
}

void AssetsLoader::load_glTFNode(const tinygltf::Node &inputNode, const tinygltf::Model &input, RenderNode::Ptr parent)
{
    RenderNode::Ptr node = RenderNode::New();
    node->Parent = parent;

    // Model matrix
    {
        if (inputNode.translation.size() == 3)
        {
            glm::vec3 translation = glm::make_vec3(inputNode.translation.data());
            node->ModelMatrix = glm::translate(node->ModelMatrix, translation);
        }
        if (inputNode.rotation.size() == 4)
        {
            glm::quat q = glm::make_quat(inputNode.rotation.data());
            glm::mat4 m = glm::mat4(q);
            node->ModelMatrix *= m;
        }
        if (inputNode.scale.size() == 3)
        {
            glm::vec3 scale = glm::make_vec3(inputNode.scale.data());
            node->ModelMatrix = glm::scale(node->ModelMatrix, scale);
        }
        if (inputNode.matrix.size() == 16)
        {
            glm::mat4 m = glm::make_mat4x4(inputNode.matrix.data());
            node->ModelMatrix = m;
        };
    }

    // Load node's children
    {
        if (inputNode.children.size() > 0)
        {
            for (size_t i = 0; i < inputNode.children.size(); i++)
            {
                load_glTFNode(input.nodes[inputNode.children[i]], input, node);
            }
        }
    }

    // Create mesh render
    {
        if (inputNode.mesh > -1)
        {
            const tinygltf::Mesh mesh = input.meshes[inputNode.mesh];
            for (size_t i = 0; i < mesh.primitives.size(); i++)
            {
                const tinygltf::Primitive &glTFPrimitive = mesh.primitives[i];

                // Vertices
                std::vector<vec3> vertices;
                std::vector<vec2> texcoords;
                std::vector<vec3> normals;
                {
                    const float* vertexBuffer = nullptr;
                    const float* texcoordBuffer = nullptr;
                    const float* normalsBuffer = nullptr;
                    size_t vertexCount = 0;

                    if (glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end())
                    {
                        const tinygltf::Accessor &accessor = input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
                        const tinygltf::BufferView &view = input.bufferViews[accessor.bufferView];
                        vertexBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                        vertexCount = accessor.count;
                    }

                    if (glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end())
                    {
                        const tinygltf::Accessor &accessor = input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
                        const tinygltf::BufferView &view = input.bufferViews[accessor.bufferView];
                        texcoordBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    }

                    if (glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end())
                    {
                        const tinygltf::Accessor &accessor = input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
                        const tinygltf::BufferView &view = input.bufferViews[accessor.bufferView];
                        normalsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    }

                    vertices.resize(vertexCount);
                    texcoords.resize(vertexCount);
                    normals.resize(vertexCount);
                    for (size_t i = 0; i < vertexCount; ++i)
                    {
                        vertices[i] = glm::make_vec3(&vertexBuffer[i * 3]);
                        texcoords[i] = texcoordBuffer ? glm::make_vec2(&texcoordBuffer[i * 2]) : glm::vec2(0.0f);
                        normals[i] = glm::normalize(normalsBuffer ? glm::make_vec3(&normalsBuffer[i * 3]) : glm::vec3(0.0f));
                    }
                }

                // Indices
                std::vector<unsigned int> indices;
                {
                    const tinygltf::Accessor &accessor = input.accessors[glTFPrimitive.indices];
                    const tinygltf::BufferView &bufferView = input.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer &buffer = input.buffers[bufferView.buffer];
                    
                    indices.resize(accessor.count);

                    switch (accessor.componentType)
                    {
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                        {
                            const unsigned int* buf = reinterpret_cast<const unsigned int*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                            for (size_t i = 0; i < accessor.count; ++i)
                            {
                                indices[i] = buf[i];
                            }
                            break;
                        }
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                        {
                            const unsigned short* buf = reinterpret_cast<const unsigned short*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                            for (size_t i = 0; i < accessor.count; ++i)
                            {
                                indices[i] = buf[i];
                            }
                            break;
                        }
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                        {
                            const unsigned char* buf = reinterpret_cast<const unsigned char*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                            for (size_t i = 0; i < accessor.count; ++i)
                            {
                                indices[i] = buf[i];
                            }
                            break;
                        }
                        default:
                            std::cerr << "Index component type " << accessor.componentType << " is not supported!" << std::endl;
                            break;
                    }
                }

                glTFMaterialData::Ptr glTFMatData = AssetsLoader::glTFMatDatas[glTFPrimitive.material];
                Material::Ptr meshMat = Material::New("PBRLit", "glsl_shaders/PBRLit.vert", "glsl_shaders/PBRLit.frag");
                if (glTFMatData->baseColorTexture)
                {
                    meshMat->addOrSetTexture("uBaseMap", glTFMatData->baseColorTexture);
                }
                meshMat->addOrSetFloat("uBaseMapSet", glTFMatData->baseColorTexture ? 1.0f : -1.0f);
                meshMat->addOrSetVector("uBaseColor", glTFMatData->baseColorFactor);

                if (glTFMatData->normalTexture)
                    meshMat->addOrSetTexture("uNormalMap", glTFMatData->normalTexture);
                meshMat->addOrSetFloat("uNormalMapSet", glTFMatData->normalTexture ? 1.0f : -1.0f);

                if (glTFMatData->emissiveTexture)
                {
                    meshMat->addOrSetTexture("uEmissiveMap", glTFMatData->emissiveTexture);
                    meshMat->addOrSetVector("uEmissiveColor", glTFMatData->emissiveFactor);
                }
                meshMat->addOrSetFloat("uEmissiveMapSet", glTFMatData->emissiveTexture ? 1.0f : -1.0f);

                if (glTFMatData->metallicRoughnessTexture)
                {
                    meshMat->addOrSetTexture("uMetallicRoughnessMap", glTFMatData->metallicRoughnessTexture);
                }
                meshMat->addOrSetFloat("uMetallicRoughnessMapSet", glTFMatData->metallicRoughnessTexture ? 1.0f : -1.0f);
                meshMat->addOrSetFloat("uMetallicFactor", glTFMatData->metallicFactor);
                meshMat->addOrSetFloat("uRoughnessFactor", glTFMatData->roughnessFactor);

                if (glTFMatData->occlusionTexture)
                {
                    meshMat->addOrSetTexture("uOcclusionMap", glTFMatData->occlusionTexture);
                }
                meshMat->addOrSetFloat("uOcclusionMapSet", glTFMatData->occlusionTexture ? 1.0f : -1.0f);

                meshMat->setDoubleSided(glTFMatData->doubleSided);

                meshMat->setAlphaMode(glTFMatData->alphaMode);
                meshMat->addOrSetFloat("uAlphaTestSet", glTFMatData->alphaMode == Material::AlphaMode::MASK ? 1.0f : -1.0f);
                meshMat->addOrSetFloat("uAlphaCutoff", glTFMatData->alphaCutoff);

                node->MeshRenders.push_back(MeshRender::New(Mesh::New(vertices, texcoords, normals, indices), meshMat));
            }
        }
    }

    parent->Children.push_back(node);
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

RenderNode::Ptr AssetsLoader::loadObj(const std::string &filePath)
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
    return AssetsLoader::processAssimpNode(scene->mRootNode, scene, directory, nullptr);
}

RenderNode::Ptr AssetsLoader::processAssimpNode(aiNode* aNode, const aiScene* aScene, const std::string& directory, RenderNode::Ptr parent)
{
    RenderNode::Ptr node = RenderNode::New();
    node->Parent = parent;
    node->ModelMatrix = AssetsLoader::aiMatrix4x4ToGlmMat4(aNode->mTransformation);

    for (size_t i = 0; i < aNode->mNumMeshes; ++i)
    {
        aiMesh* assimpMesh = aScene->mMeshes[aNode->mMeshes[i]];
        aiMaterial* assimpMat = aScene->mMaterials[assimpMesh->mMaterialIndex];

        Mesh::Ptr mesh = AssetsLoader::parseMesh(assimpMesh, aScene);
        Material::Ptr mat = AssetsLoader::parseMaterial(assimpMat, aScene, directory);

        node->MeshRenders.push_back(MeshRender::New(mesh, mat));
    }

    // also recursively parse this node's children 
    for (unsigned int i = 0; i < aNode->mNumChildren; ++i)
    {
        node->Children.push_back(AssetsLoader::processAssimpNode(aNode->mChildren[i], aScene, directory, node));
    }

    return node;
}

Mesh::Ptr AssetsLoader::parseMesh(aiMesh* aMesh, const aiScene* aScene)
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
    }

    for (size_t f = 0; f < aMesh->mNumFaces; ++f)
    {
        aiFace face = aMesh->mFaces[f];
        for (size_t i = 0; i < 3; ++i)
        {
            indices[3 * f + i] = face.mIndices[i];
        }
    }

    return Mesh::New(vertices, texcoords, normals, indices);
}

Material::Ptr AssetsLoader::parseMaterial(aiMaterial* aMaterial, const aiScene* aScene, const std::string& directory)
{
//    Material::Ptr mat = Material::New("Blinn-Phong", "glsl_shaders/BlinnPhong.vert", "glsl_shaders/BlinnPhong.frag");
    Material::Ptr mat = Material::New("PBR", "glsl_shaders/PBRLit.vert", "glsl_shaders/PBRLit.frag");

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
    {
        mat->addOrSetVector("uBaseColor", glm::vec4(color.r, color.g, color.b, color.a));
    }
    else
    {
        mat->addOrSetVector("uBaseColor", glm::vec4(1.0f));
    }

    // Normal map
    if (AI_SUCCESS == aMaterial->GetTexture(aiTextureType_NORMALS, 0, &texturePath))
    {
        mat->addOrSetTexture(AssetsLoader::loadAssimpTexture("uNormalMap", directory, texturePath.C_Str()));
        mat->addOrSetFloat("uNormalMapSet", 1.0f);
    }
    else
    {
        mat->addOrSetFloat("uNormalMapSet", -1.0f);
    }

    // Emission
    if (AI_SUCCESS == aMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath))
    {
        mat->addOrSetTexture(AssetsLoader::loadAssimpTexture("uEmissiveMap", directory, texturePath.C_Str()));
        mat->addOrSetFloat("uEmissiveMapSet", 1.0f);
    }
    else
    {
        mat->addOrSetFloat("uEmissiveMapSet", -1.0f);
    }
    
    // Emission color
    if (AI_SUCCESS == aiGetMaterialColor(aMaterial, AI_MATKEY_COLOR_EMISSIVE, &color))
    {
        mat->addOrSetVector("uEmissiveColor", glm::vec3(color.r, color.g, color.b));
    }
    else
    {
        mat->addOrSetVector("uEmissiveColor", glm::vec3(1.0f));
    }

    // Metallic roughness texture
    // aiTextureType_METALNESS or aiTextureType_DIFFUSE_ROUGHNESS
    if (AI_SUCCESS == aMaterial->GetTexture(aiTextureType_METALNESS, 0, &texturePath))
    {
        mat->addOrSetTexture(AssetsLoader::loadAssimpTexture("uMetallicRoughnessMap", directory, texturePath.C_Str()));
        mat->addOrSetFloat("uMetallicRoughnessMapSet", 1.0f);
    }
    else
    {
        mat->addOrSetFloat("uMetallicRoughnessMapSet", -1.0f);
    }
    
    // Metallic factor
    ai_real valueFactor;
    if (AI_SUCCESS == aiGetMaterialFloat(aMaterial, AI_MATKEY_METALLIC_FACTOR, &valueFactor))
    {
        mat->addOrSetFloat("uMetallicFactor", valueFactor);
    }
    else
    {
        mat->addOrSetFloat("uMetallicFactor", 0.0f);
    }
    
    // Roughness factor
    if (AI_SUCCESS == aiGetMaterialFloat(aMaterial, AI_MATKEY_ROUGHNESS_FACTOR, &valueFactor))
    {
        mat->addOrSetFloat("uRoughnessFactor", valueFactor);
    }
    else
    {
        mat->addOrSetFloat("uRoughnessFactor", 0.8f);
    }

    // Occlusion map
    if (AI_SUCCESS == aMaterial->GetTexture(aiTextureType_LIGHTMAP, 0, &texturePath))
    {
        mat->addOrSetTexture(AssetsLoader::loadAssimpTexture("uOcclusionMap", directory, texturePath.C_Str()));
        mat->addOrSetFloat("uOcclusionMapSet", 1.0f);
    }
    else
    {
        mat->addOrSetFloat("uOcclusionMapSet", -1.0f);
    }
    
    // Cull face
    int two_sided;
    if((AI_SUCCESS == aiGetMaterialInteger(aMaterial, AI_MATKEY_TWOSIDED, &two_sided)) && two_sided)
    {
        mat->setDoubleSided(true);
    }
    else
    {
        mat->setDoubleSided(false);
    }
    
    aiString alphaMode("OPAQUE");
    Material::AlphaMode mode = Material::AlphaMode::DEFAULT_OPAQUE;
    if (AI_SUCCESS == aiGetMaterialString(aMaterial, AI_MATKEY_GLTF_ALPHAMODE, &alphaMode))
    {
        std::string m = alphaMode.C_Str();
        if (m == "MASK")
        {
            mode = Material::AlphaMode::MASK;
        }
        else if (m == "BLEND")
        {
            mode = Material::AlphaMode::BLEND;
        }
    }
    mat->setAlphaMode(mode);
    mat->addOrSetFloat("uAlphaTestSet", mode == Material::AlphaMode::MASK ? 1.0f : -1.0f);
    
    // Alpha cuteoff
    if (AI_SUCCESS == aiGetMaterialFloat(aMaterial, AI_MATKEY_GLTF_ALPHACUTOFF, &valueFactor))
    {
        mat->addOrSetFloat("uAlphaCutoff", valueFactor);
    }
    else
    {
        mat->addOrSetFloat("uAlphaCutoff", 0.5f);
    }

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
