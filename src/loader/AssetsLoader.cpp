#include "loader/AssetsLoader.h"

#include <fstream>
#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>

#include <ktx.h>

#include "base/Material.h"

std::vector<AssetsLoader::glTFMaterialData::Ptr> AssetsLoader::glTFMatDatas = {};

Shader::Ptr AssetsLoader::loadShaderFromFile(const std::string &name, const std::string &vsFilePath, const std::string &fsFilePath)
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

Texture::Ptr AssetsLoader::loadTextureFromFile(const std::string& textureName, const std::string& filePath, bool useMipmap)
{
    Texture::Ptr texture = Texture::New();

    stbi_set_flip_vertically_on_load(true);

    std::string newPath = getAssetsPath() + filePath;
    int width, height, components;
    unsigned char* data = stbi_load(newPath.c_str(), &width, &height, &components, 0);
    if (data)
    {
        GLenum format = getFormat(components);
        texture->initTexture(textureName, width, height, components, format, data, useMipmap);
    }
    else
    {
        std::cerr << "Failed to load texture: " << newPath << std::endl;
    }

    stbi_image_free(data);

    stbi_set_flip_vertically_on_load(false);

    return texture;
}

Texture::Ptr AssetsLoader::createTextureFromBuffer(const std::string &textureName, const int &width, const int &height, const int &components, void* buffer, bool useMipmap)
{
    Texture::Ptr texture = Texture::New();

    if (buffer)
    {
        GLenum format = getFormat(components);
        texture->initTexture(textureName, width, height, components, format, buffer, useMipmap);
    }
    else
    {
        std::cerr << "Failer to create texture: "<< textureName << "! Texture buffer is nullptr." << std::endl;
    }

    return texture;
}

RenderNode::Ptr AssetsLoader::loadglTFFile(const std::string& filePath)
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
        loadglTFMaterials(glTFInput, rootNode);

        const tinygltf::Scene& scene = glTFInput.scenes[0];
        for (size_t i = 0; i < scene.nodes.size(); ++i)
        {
            const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
            loadglTFNode(node, glTFInput, rootNode);
        }
    }
    else
    {
        std::cerr << "Could not open the glTF file: " << newPath << ", error: " << error << std::endl;
    }

    return rootNode;
}

Texture::Ptr AssetsLoader::loadTextureFromKTXFile(const std::string &textureName, const std::string &filePath, bool useMipmap)
{
    ktxTexture* kTexture;
    KTX_error_code result;

    std::string newPath = getAssetsPath() + filePath;
    result = ktxTexture_CreateFromNamedFile(newPath.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &kTexture);

    assert(result == KTX_SUCCESS);

    Texture::Ptr texture = Texture::New();
    texture->initTexture(textureName, kTexture, useMipmap);
    ktxTexture_Destroy(kTexture);
    return texture;
}

void AssetsLoader::loadglTFMaterials(const tinygltf::Model &input, RenderNode::Ptr rootNode)
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
    //std::vector<Texture::Ptr> textures;
    rootNode->NodeTextures.clear();
    {
        rootNode->NodeTextures.resize(input.images.size());
        for (size_t index = 0; index < input.images.size(); ++index)
        {
            const tinygltf::Image &glTFImage = input.images[index];
            unsigned char *buffer = const_cast<unsigned char *>(&glTFImage.image[0]);
            rootNode->NodeTextures[index] = createTextureFromBuffer(glTFImage.name, glTFImage.width, glTFImage.height, glTFImage.component, reinterpret_cast<void *>(buffer));
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

void AssetsLoader::loadglTFNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, RenderNode::Ptr parent)
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
                loadglTFNode(input.nodes[inputNode.children[i]], input, node);
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
                const tinygltf::Primitive& glTFPrimitive = mesh.primitives[i];

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
                        const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
                        const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                        vertexBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                        vertexCount = accessor.count;
                    }

                    if (glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end())
                    {
                        const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
                        const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                        texcoordBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    }

                    if (glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end())
                    {
                        const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
                        const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                        normalsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    }

                    for (size_t i = 0; i < vertexCount; ++i)
                    {
                        vertices.push_back(glm::make_vec3(&vertexBuffer[i * 3]));
                        texcoords.push_back(texcoordBuffer ? glm::make_vec2(&texcoordBuffer[i * 2]) : glm::vec2(0.0f));
                        normals.push_back(glm::normalize(normalsBuffer ? glm::make_vec3(&normalsBuffer[i * 3]) : glm::vec3(0.0f)));
                    }
                }

                // Indices
                std::vector<unsigned int> indices;
                {
                    const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.indices];
                    const tinygltf::BufferView& bufferView = input.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = input.buffers[bufferView.buffer];

                    switch (accessor.componentType)
                    {
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                    {
                        const unsigned int* buf = reinterpret_cast<const unsigned int*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                        for (size_t i = 0; i < accessor.count; ++i)
                        {
                            indices.push_back(buf[i]);
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                    {
                        const unsigned short* buf = reinterpret_cast<const unsigned short*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                        for (size_t i = 0; i < accessor.count; ++i)
                        {
                            indices.push_back(buf[i]);
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                    {
                        const unsigned char* buf = reinterpret_cast<const unsigned char*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                        for (size_t i = 0; i < accessor.count; ++i)
                        {
                            indices.push_back(buf[i]);
                        }
                        break;
                    }
                    default:
                        std::cerr << "Index component type " << accessor.componentType << " is not supported!" << std::endl;
                        break;
                    }
                }

                glTFMaterialData::Ptr glTFMatData = AssetsLoader::glTFMatDatas[glTFPrimitive.material];

                Material::Ptr newMat = Material::New("PBRLit", "glsl_shaders/PBRLit.vs", "glsl_shaders/PBRLit.fs");
                if (glTFMatData->baseColorTexture)
                {
                    newMat->addOrSetTexture("uAlbedoMap", glTFMatData->baseColorTexture);
                }
                newMat->addOrSetFloat("uAlbedoMapSet", glTFMatData->baseColorTexture ? 1.0f : -1.0f);
                newMat->addOrSetVector("uBaseColor", glTFMatData->baseColorFactor);

                if (glTFMatData->normalTexture)
                    newMat->addOrSetTexture("uNormalMap", glTFMatData->normalTexture);
                newMat->addOrSetFloat("uNormalMapSet", glTFMatData->normalTexture ? 1.0f : -1.0f);

                if (glTFMatData->emissiveTexture)
                {
                    newMat->addOrSetTexture("uEmissiveMap", glTFMatData->emissiveTexture);
                    newMat->addOrSetVector("uEmissiveColor", glTFMatData->emissiveFactor);
                }
                newMat->addOrSetFloat("uEmissiveMapSet", glTFMatData->emissiveTexture ? 1.0f : -1.0f);

                if (glTFMatData->metallicRoughnessTexture)
                {
                    newMat->addOrSetTexture("uMetallicRoughnessMap", glTFMatData->metallicRoughnessTexture);
                }
                newMat->addOrSetFloat("uMetallicRoughnessMapSet", glTFMatData->metallicRoughnessTexture ? 1.0f : -1.0f);
                newMat->addOrSetFloat("uMetallicFactor", glTFMatData->metallicFactor);
                newMat->addOrSetFloat("uRoughnessFactor", glTFMatData->roughnessFactor);

                if (glTFMatData->occlusionTexture)
                {
                    newMat->addOrSetTexture("uOcclusionMap", glTFMatData->occlusionTexture);
                }
                newMat->addOrSetFloat("uOcclusionMapSet", glTFMatData->occlusionTexture ? 1.0f : -1.0f);

                newMat->setDoubleSided(glTFMatData->doubleSided);

                newMat->setAlphaMode(glTFMatData->alphaMode);
                newMat->addOrSetFloat("uAlphaTestSet", glTFMatData->alphaMode == Material::AlphaMode::MASK ? 1.0f : -1.0f);
                newMat->addOrSetFloat("uAlphaCutoff", glTFMatData->alphaCutoff);

                node->MeshRenders.push_back(MeshRender::New(Mesh::New(vertices, texcoords, normals, indices), newMat));
            }
        }
    }

    parent->Children.push_back(node);
}

std::string AssetsLoader::readShader(std::ifstream& file, const std::string& name)
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