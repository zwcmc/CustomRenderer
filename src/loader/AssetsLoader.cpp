#include "loader/AssetsLoader.h"

#include <fstream>
#include <stb_image.h>

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

Texture::Ptr AssetsLoader::loadTexture(const std::string& textureName, const std::string& filePath, bool flipVertical, bool useMipmap)
{
    stbi_set_flip_vertically_on_load(flipVertical);

    std::string newPath = getAssetsPath() + filePath;

    Texture::Ptr texture = Texture::New();

    int width, height, components;
    unsigned char* data = stbi_load(newPath.c_str(), &width, &height, &components, 0);
    if (data)
    {
        GLenum format;
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
        texture->initTexture(textureName, width, height, components, format, data, useMipmap);
    }
    else
    {
        std::cerr << "Failed to load texture: " << newPath << std::endl;
    }

    stbi_image_free(data);

    return texture;
}

glTFRenderer::Ptr AssetsLoader::loadglTFFile(const std::string &filePath)
{
    std::string newPath = getAssetsPath() + filePath;

    tinygltf::Model glTFInput;
    tinygltf::TinyGLTF glTFContext;
    std::string error, warning;

    glTFRenderer::Ptr renderer = glTFRenderer::New();

    bool fileLoaded = glTFContext.LoadASCIIFromFile(&glTFInput, &error, &warning, newPath);
    if (fileLoaded)
    {
        const tinygltf::Scene &scene = glTFInput.scenes[0];
        for (size_t i = 0; i < scene.nodes.size(); ++i)
        {
            const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
            loadglTFNode(node, glTFInput, renderer, nullptr);
        }
    }
    else
    {
        std::cerr << "Could not open the glTF file: " << newPath << ", error: " << error << std::endl;
    }

    return renderer;
}

void AssetsLoader::loadglTFNode(const tinygltf::Node &inputNode, const tinygltf::Model &input, glTFRenderer::Ptr renderer, glTFRenderer::glTFNode::Ptr parent)
{
    glTFRenderer::glTFNode::Ptr node = glTFRenderer::glTFNode::New();
    node->parent = parent;

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

    if (inputNode.children.size() > 0)
    {
        for (size_t i = 0; i < inputNode.children.size(); i++)
        {
            loadglTFNode(input.nodes[inputNode.children[i]], input, renderer, node);
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
            {
                const float* vertexBuffer = nullptr;
                const float* texcoordBuffer = nullptr;
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

                for (size_t i = 0; i < vertexCount; ++i)
                {
                    vertices.push_back(glm::make_vec3(&vertexBuffer[i * 3]));
                    texcoords.push_back(texcoordBuffer ? glm::make_vec2(&texcoordBuffer[i * 2]) : glm::vec2(0.0f));
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

            node->meshes.push_back(Mesh::New(vertices, texcoords, indices));
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