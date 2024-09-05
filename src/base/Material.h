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
    enum AlphaMode
    {
        DEFAULT_OPAQUE = 0,
        MASK,
        BLEND
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

    void SetDoubleSided(bool bDoubleSided);
    bool GetDoubleSided() { return m_DoubleSided; }

    void SetAlphaMode(AlphaMode mode);
    AlphaMode GetAlphaMode() { return m_AlphaMode; }

    bool IsUsedForSkybox() { return m_UsedForSkybox; }

    void SetCastShadows(bool cast) { m_CastShadows  = cast; }
    bool GetMaterialCastShadows() { return m_CastShadows; }
    
    std::string GetShaderName() { return m_Shader->GetName(); }

    void Use();
    
    void ClearUniforms();

private:
    Shader::Ptr m_Shader;

    std::map<std::string, Texture2D::Ptr> m_Textures;
    std::map<std::string, TextureCube::Ptr> m_TextureCubes;
    std::map<std::string, glm::vec4> m_UniformVec4;
    std::map<std::string, float> m_UniformFloats;

    bool m_UsedForSkybox;
    bool m_DoubleSided;
    AlphaMode m_AlphaMode;
    bool m_CastShadows;
};
