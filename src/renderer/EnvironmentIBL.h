#pragma once

#include <string>
#include <glad/glad.h>

#include "ptr.h"
#include "base/Texture2D.h"
#include "base/TextureCube.h"
#include "renderer/RenderTarget.h"
#include "scene/SceneNode.h"

class EnvironmentIBL
{
    SHARED_PTR(EnvironmentIBL)
public:

    EnvironmentIBL(const std::string &cubemapPath, const GLuint &uniformBufferID);
    ~EnvironmentIBL();

    void LoadEnvironmentCubemap(const std::string& cubemapPath);
    void GenerateCubemaps();
    void GenerateBRDFLUT();

    void RenderToCubemap(TextureCube::Ptr cubemap, unsigned int mipLevel = 0);
    void DrawSceneNode(SceneNode::Ptr sceneNode);

    SceneNode::Ptr GetSkyboxRenderNode();
    TextureCube::Ptr GetIrradiance();
    TextureCube::Ptr GetPrefiltered();
    Texture2D::Ptr GetBRDFLUTTexture();

    GLuint m_UniformBufferID;

    GLuint m_FrameBufferID, m_DepthRenderBufferID;
    SceneNode::Ptr m_Cube;
    TextureCube::Ptr m_EnvironmentCubemap;
    TextureCube::Ptr m_IrradianceCubemap;
    TextureCube::Ptr m_PrefilteredCubemap;
    RenderTarget::Ptr m_BRDFLUTRenderTarget;
};