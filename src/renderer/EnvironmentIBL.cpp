#include "renderer/EnvironmentIBL.h"

#include <iostream>

#include <glm/glm.hpp>

#include "defines.h"
#include "loader/AssetsLoader.h"
#include "cameras/Camera.h"

#include "renderer/Blitter.h"

EnvironmentIBL::EnvironmentIBL(const std::string &cubemapPath, const GLuint &uniformBufferID)
{
    m_UniformBufferID = uniformBufferID;

    // Framebuffer and render buffer for off-screen rendering cubemaps
    glGenFramebuffers(1, &m_FrameBufferID);
    glGenRenderbuffers(1, &m_DepthRenderBufferID);

    // Mesh for cubemap
    m_Cube = AssetsLoader::LoadModel("models/glTF/Box/glTF-Binary/Box.glb", false);
    
    m_SkyboxMat = Material::New("Skybox", "environment/Cube.vs", "environment/Skybox.fs", true);
    m_SkyboxMat->SetCastShadows(false);

    // Load environment cubemap
    LoadEnvironmentCubemap(cubemapPath);
    // Generate irradiance cubemap and pre-filtered cuebmap
    GenerateCubemaps();
    // Generate environment BRDF look-up table
    GenerateBRDFLUT();
}

EnvironmentIBL::~EnvironmentIBL()
{
    glDeleteFramebuffers(1, &m_FrameBufferID);
    glDeleteRenderbuffers(1, &m_DepthRenderBufferID);
}

void EnvironmentIBL::LoadEnvironmentCubemap(const std::string &cubemapPath)
{
    m_EnvironmentCubemap = TextureCube::New("uEnvironmentCubemap");

    std::string fileExt;
    size_t extPos = cubemapPath.rfind('.', cubemapPath.length());
    if (extPos != std::string::npos)
    {
        fileExt = cubemapPath.substr(extPos + 1, cubemapPath.length());
    }
    else
    {
        std::cerr << "Cubemap file path is wrong, path is: " << cubemapPath << std::endl;
        return;
    }

    if (fileExt == "hdr")
    {
        Texture2D::Ptr environmentMap = AssetsLoader::LoadHDRTexture("uHDRMap", cubemapPath);
        m_EnvironmentCubemap->DefaultInit(environmentMap->GetSize().y, environmentMap->GetSize().y, GL_RGB16F, GL_RGB, GL_HALF_FLOAT);

        // Equirectangular map to a cubemap
        Material::Ptr capMat = Material::New("HDR_to_Cubemap", "environment/Cube.vs", "environment/HDRToCubemap.fs");
        capMat->AddOrSetTexture(environmentMap);
        m_Cube->SetOverrideMaterial(capMat);
        RenderToCubemap(m_EnvironmentCubemap, 0);
    }
    else
    {
        std::cerr << "Unsupport cubemap file, path is: " << cubemapPath << std::endl;
        return;
    }
    
    m_SkyboxMat->AddOrSetTextureCube(m_EnvironmentCubemap);
}

void EnvironmentIBL::GenerateCubemaps()
{
    // Diffuse irradiance
    m_IrradianceCubemap = TextureCube::New("uIrradianceCubemap");
    m_IrradianceCubemap->DefaultInit(32, 32, GL_RGB16F, GL_RGB, GL_HALF_FLOAT);

    Material::Ptr cubemapConvolutionMat = Material::New("Cubemap_Convolution", "environment/Cube.vs", "environment/IrradianceCubemap.fs");
    cubemapConvolutionMat->AddOrSetTextureCube(m_EnvironmentCubemap);
    m_Cube->SetOverrideMaterial(cubemapConvolutionMat);
    RenderToCubemap(m_IrradianceCubemap, 0);

    // Specular IBL
    m_PrefilteredCubemap = TextureCube::New("uPrefilteredCubemap");
    m_PrefilteredCubemap->DefaultInit(512, 512, GL_RGB16F, GL_RGB, GL_HALF_FLOAT, true);

    Material::Ptr cubemapPrefilteredMat = Material::New("Cubemap_Prefiltered", "environment/Cube.vs", "environment/PrefilteredCubemap.fs");
    cubemapPrefilteredMat->AddOrSetTextureCube(m_EnvironmentCubemap);
    m_Cube->SetOverrideMaterial(cubemapPrefilteredMat);

    const uint32_t numMips = static_cast<uint32_t>(floor(std::log2(512))) + 1;
    std::cout << "Pre-filtered environment cubemap mipmap nums is: " << numMips << std::endl;
    for (unsigned int mip = 0; mip < numMips; ++mip)
    {
        cubemapPrefilteredMat->AddOrSetFloat("uRoughness", (float)(mip) / (numMips - 1));
        RenderToCubemap(m_PrefilteredCubemap, mip);
    }
}

void EnvironmentIBL::GenerateBRDFLUT()
{
    m_BRDFLUTRenderTarget = RenderTarget::New(128, 128, GL_HALF_FLOAT, 1);
    m_BRDFLUTRenderTarget->GetColorTexture(0)->SetTextureName("uBRDFLUT");
    Material::Ptr generateBRDFLUTFMat = Material::New("Generate_BRDF_LUT", "utils/FullScreenTriangle.vs", "environment/GenerateBRDFLUT.fs");

    Blitter::RenderToTarget(m_BRDFLUTRenderTarget, generateBRDFLUTFMat);
}

void EnvironmentIBL::RenderToCubemap(TextureCube::Ptr cubemap, unsigned int mipLevel)
{
    int width = cubemap->GetSize().x >> mipLevel;
    int height = cubemap->GetSize().y >> mipLevel;

    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRenderBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthRenderBufferID);
    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "FrameBuffer is not complete!" << std::endl;
    }
    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glm::mat4 captureProjection = glm::perspective((float)M_PI / 2.0f, (float)width / height, 0.1f, 10.0f);
    Camera::Ptr faceCameras[6] =
    {
        Camera::New(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        Camera::New(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        Camera::New(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        Camera::New(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        Camera::New(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        Camera::New(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    // Configure the viewport to the capture dimensions
    glViewport(0, 0, width, height);

    // Bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);

    // Vertex shader output gl_Postion = clipPos.xyww, depth is maximum 1.0, so Use less&equal depth func
    glDepthFunc(GL_LEQUAL);

    // Importent: Render the spherical position to the cube position inside the inner cube box; cull face must be disabled
    glDisable(GL_CULL_FACE);

    // Bind global uniforms
    glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBufferID);

    glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &(captureProjection[0].x));

    for (unsigned int i = 0; i < 6; ++i)
    {
        // Set global uniforms
        glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &(faceCameras[i]->GetViewMatrix()[0].x));

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap->GetTextureID(), mipLevel);
        // Check framebuffer status
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "FrameBuffer is not complete in rendering 6 faces!" << mipLevel << std::endl;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        DrawSceneNode(m_Cube);
    }

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Unbind global uniforms
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Set back to default
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
}

void EnvironmentIBL::DrawSceneNode(SceneNode::Ptr node)
{
    Material::Ptr overrideMat = node->OverrideMat;
    for (size_t i = 0; i < node->MeshRenders.size(); ++i)
    {
        MeshRender::Ptr mr = node->MeshRenders[i];
        if (overrideMat)
        {
            overrideMat->Use();
        }
        else
        {
            mr->GetMaterial()->Use();
        }

        Mesh::Ptr mesh = mr->GetMesh();
        glBindVertexArray(mesh->GetVertexArrayID());

        if (mesh->GetIndicesCount() > 0)
        {
            glDrawElements(GL_TRIANGLES, mesh->GetIndicesCount(), GL_UNSIGNED_INT, nullptr);
        }
        else
        {
            glDrawArrays(GL_TRIANGLES, 0, mesh->GetVerticesCount());
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }

    for (size_t i = 0; i < node->GetChildrenCount(); ++i)
    {
        DrawSceneNode(node->GetChildByIndex(i));
    }
}

SceneNode::Ptr EnvironmentIBL::GetSkyboxRenderNode()
{
    m_Cube->SetOverrideMaterial(m_SkyboxMat);
    return m_Cube;
}

TextureCube::Ptr EnvironmentIBL::GetIrradiance()
{
    return m_IrradianceCubemap;
}

TextureCube::Ptr EnvironmentIBL::GetPrefiltered()
{
    return m_PrefilteredCubemap;
}

Texture2D::Ptr EnvironmentIBL::GetBRDFLUTTexture()
{
    return m_BRDFLUTRenderTarget->GetColorTexture(0);
}
