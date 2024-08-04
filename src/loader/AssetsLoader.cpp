#include "loader/AssetsLoader.h"

#include <fstream>
#include <stb_image.h>

std::vector<glTFRenderer::glTFMaterialData::Ptr> AssetsLoader::glTFMatDatas = {};

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

glTFRenderer::Ptr AssetsLoader::loadglTFFile(const std::string &filePath, Material::Ptr mat)
{
    std::string newPath = getAssetsPath() + filePath;

    tinygltf::Model glTFInput;
    tinygltf::TinyGLTF glTFContext;
    std::string error, warning;

    glTFRenderer::Ptr renderer = glTFRenderer::New();

    bool fileLoaded = glTFContext.LoadASCIIFromFile(&glTFInput, &error, &warning, newPath);
    if (fileLoaded)
    {
        // Load texture and material data
        loadglTFMaterials(glTFInput);

        const tinygltf::Scene &scene = glTFInput.scenes[0];
        for (size_t i = 0; i < scene.nodes.size(); ++i)
        {
            const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
            loadglTFNode(node, glTFInput, renderer, nullptr, mat);
        }
    }
    else
    {
        std::cerr << "Could not open the glTF file: " << newPath << ", error: " << error << std::endl;
    }

    return renderer;
}

void AssetsLoader::loadglTFMaterials(const tinygltf::Model &input)
{
    // Load texture indices
    std::vector<int> textureIndices;
    {
        textureIndices.resize(input.textures.size());
        for (size_t index = 0; index < input.textures.size(); ++index) {
            textureIndices[index] = input.textures[index].source;
        }
    }

    // Create textures
    std::vector<Texture::Ptr> textures;
    {
        textures.resize(input.images.size());
        for (size_t index = 0; index < input.images.size(); ++index)
        {
            const tinygltf::Image& glTFImage = input.images[index];
            unsigned char* buffer = const_cast<unsigned char*>(&glTFImage.image[0]);
            textures[index] = createTextureFromBuffer(glTFImage.name, glTFImage.width, glTFImage.height, glTFImage.component, reinterpret_cast<void*>(buffer));
        }
    }

    // load glTF materials
    AssetsLoader::glTFMatDatas.clear();
    {
        AssetsLoader::glTFMatDatas.resize(input.materials.size());
        for (size_t index = 0; index < input.materials.size(); ++index)
        {
            AssetsLoader::glTFMatDatas[index] = glTFRenderer::glTFMaterialData::New();

            tinygltf::Material glTFMaterial = input.materials[index];
            if (glTFMaterial.values.find("baseColorTexture") != glTFMaterial.values.end())
            {
                int textureIndex = glTFMaterial.values["baseColorTexture"].TextureIndex();
                AssetsLoader::glTFMatDatas[index]->baseColorTexture = textures[textureIndices[textureIndex]];
            }

            if (glTFMaterial.values.find("baseColorFactor") != glTFMaterial.values.end())
            {
                AssetsLoader::glTFMatDatas[index]->baseColorFactor = glm::make_vec4(glTFMaterial.values["baseColorFactor"].ColorFactor().data());
            }
        }
    }
}

void AssetsLoader::loadglTFNode(const tinygltf::Node &inputNode, const tinygltf::Model &input, glTFRenderer::Ptr renderer, glTFRenderer::glTFNode::Ptr parent, Material::Ptr mat)
{
    glTFRenderer::glTFNode::Ptr node = glTFRenderer::glTFNode::New();
    node->parent = parent;

    // Model matrix
    if (inputNode.translation.size() == 3)
    {
        glm::vec3 translation = glm::make_vec3(inputNode.translation.data());
        node->matrix = glm::translate(node->matrix, translation);
    }
    if (inputNode.rotation.size() == 4)
    {
        glm::quat q = glm::make_quat(inputNode.rotation.data());
        glm::mat4 m = glm::mat4(q);
        node->matrix *= m;
    }
    if (inputNode.scale.size() == 3)
    {
        glm::vec3 scale = glm::make_vec3(inputNode.scale.data());
        node->matrix = glm::scale(node->matrix, scale);
    }
    if (inputNode.matrix.size() == 16)
    {
        glm::mat4 m = glm::make_mat4x4(inputNode.matrix.data());
        node->matrix = m;
    };

    // Load node's children
    if (inputNode.children.size() > 0)
    {
        for (size_t i = 0; i < inputNode.children.size(); i++)
        {
            loadglTFNode(input.nodes[inputNode.children[i]], input, renderer, node, mat);
        }
    }

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
                const tinygltf::Accessor &accessor = input.accessors[glTFPrimitive.indices];
                const tinygltf::BufferView &bufferView = input.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer = input.buffers[bufferView.buffer];

                switch (accessor.componentType)
                {
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                    {
                        const unsigned int *buf = reinterpret_cast<const unsigned int *>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                        for (size_t i = 0; i < accessor.count; ++i)
                        {
                            indices.push_back(buf[i]);
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                    {
                        const unsigned short *buf = reinterpret_cast<const unsigned short *>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                        for (size_t i = 0; i < accessor.count; ++i)
                        {
                            indices.push_back(buf[i]);
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                    {
                        const unsigned char *buf = reinterpret_cast<const unsigned char *>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
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

            glTFRenderer::glTFMaterialData::Ptr glTFMatData = AssetsLoader::glTFMatDatas[glTFPrimitive.material];

            if (glTFMatData->baseColorTexture)
                mat->addTextureProperty("uAlbedoMap", glTFMatData->baseColorTexture);

            mat->addFloatProperty("uAlbedoMapSet", glTFMatData->baseColorTexture ? 1.0f : -1.0f);
            mat->addVectorProperty("uBaseColor", glTFMatData->baseColorFactor);

            node->meshRenders.push_back(MeshRender::New(Mesh::New(vertices, texcoords, normals, indices), mat));
        }
    }

    if (parent)
    {
        parent->children.push_back(node);
    }
    else
    {
        renderer->addNode(node);
    }
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