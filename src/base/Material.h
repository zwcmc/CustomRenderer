#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>

#include "ptr.h"
#include "base/Shader.h"
#include "base/Texture2D.h"
#include "base/TextureCube.h"

class Material
{
    SHARED_PTR(Material)
public:
    enum class AlphaMode
    {
        DEFAULT_OPAQUE = 0,
        MASK,
        BLEND
    };
    
    enum class RenderFace
    {
        FRONT = 0,
        BACK,
        BOTH
    };

    Material(const std::string &shaderName, const std::string &vsPath, const std::string &fsPath, bool usedForSkybox = false);
    ~Material();

    void AddOrSetTexture(Texture2D::Ptr texture);
    void AddOrSetTextureCube(TextureCube::Ptr textureCube);
    void AddOrSetTexture(const std::string &propertyName, Texture2D::Ptr texture);
    void AddOrSetVector(const std::string &propertyName, const glm::vec4 &value);
    void AddOrSetFloat(const std::string &propertyName, const float &value);

    void SetMatrix(const std::string &propertyName, const glm::mat3x3 &value);
    void SetMatrix(const std::string &propertyName, const glm::mat4x4& value);

    void SetRenderFace(RenderFace face);
    Material::RenderFace GetRenderFace();

    void SetAlphaMode(AlphaMode mode);
    Material::AlphaMode GetAlphaMode();

    bool IsUsedForSkybox();

    void SetCastShadows(bool cast);
    bool GetMaterialCastShadows();

    void Use();
    
    void ClearUniforms();
    
    Shader::Ptr GetShader();

private:
    Shader::Ptr m_Shader;

    std::map<std::string, Texture2D::Ptr> m_Textures;
    std::map<std::string, TextureCube::Ptr> m_TextureCubes;
    std::map<std::string, glm::vec4> m_UniformVec4;
    std::map<std::string, float> m_UniformFloats;

    bool m_UsedForSkybox;
    bool m_CastShadows;
    
    RenderFace m_RenderFace;
    AlphaMode m_AlphaMode;
};
