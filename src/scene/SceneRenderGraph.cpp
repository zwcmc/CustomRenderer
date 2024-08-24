#include "scene/SceneRenderGraph.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "defines.h"
#include "base/TextureCube.h"
#include "loader/AssetsLoader.h"
#include "meshes/AABBCube.h"

#include "utility/Collision.h"

using namespace Collision;

SceneRenderGraph::SceneRenderGraph()
    : m_GlobalUniformBufferID(0), m_CullFace(true), m_Blend(false), m_RenderSize(glm::u32vec2(1))
{ }

SceneRenderGraph::~SceneRenderGraph()
{
    glDeleteVertexArrays(1, &m_BlitVAO);

    m_Lights.clear();
}

void SceneRenderGraph::Init()
{
    m_Scene = SceneNode::New();

    m_CommandBuffer = CommandBuffer::New();

    // No seams at cubemap edges
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // Set clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Depth test
    glEnable(GL_DEPTH_TEST);

    // Default cull face state
    m_CullFace = true;
    glEnable(GL_CULL_FACE);

    // Default blend state
    m_Blend = false;
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Global uniform buffer object
    glGenBuffers(1, &m_GlobalUniformBufferID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_GlobalUniformBufferID);
    glBufferData(GL_UNIFORM_BUFFER, 240, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_GlobalUniformBufferID); // Set global uniform to binding point 0
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Mesh for rendering lights
    m_LightMesh = Sphere::New(16, 16, 0.02f);

    glGenVertexArrays(1, &m_BlitVAO);
    m_BlitMat = Material::New("Blit", "glsl_shaders/Blit.vert", "glsl_shaders/Blit.frag");
    m_IntermediateRT = RenderTarget::New(1, 1, GL_HALF_FLOAT, 1, true);

    // Load environment cubemaps
    LoadEnvironment("textures/environments/ktx/papermill.ktx");

    // Main light shadowmap
    m_ShadowmapRT = RenderTarget::New(2048, 2048, GL_FLOAT, 0, false, true);
    m_ShadowCasterMat = Material::New("ShadowCaster", "glsl_shaders/ShadowCaster.vert", "glsl_shaders/ShadowCaster.frag");

    m_DebuggingAABBMat = Material::New("Draw AABB", "glsl_shaders/utils/DrawBoundingBox.vert", "glsl_shaders/utils/DrawBoundingBox.frag");
    m_DebuggingAABBMat->SetDoubleSided(true);
}

void SceneRenderGraph::SetRenderSize(const int &width, const int &height)
{
    m_RenderSize.x = width;
    m_RenderSize.y = height;

    m_Camera->SetScreenSize(width, height);

    m_IntermediateRT->SetSize(glm::u32vec2(width, height));
}

void SceneRenderGraph::SetCamera(Camera::Ptr camera)
{
    m_Camera = camera;
}

void SceneRenderGraph::AddLight(Light::Ptr light)
{
    m_Lights.push_back(light);

    // Add a new render command for render light
    AddRenderLightCommand(light);
}

void SceneRenderGraph::AddSceneNode(SceneNode::Ptr sceneNode)
{
    m_Scene->AddChild(sceneNode);

    // Build render commands
    BuildRenderCommands(sceneNode);
}

void SceneRenderGraph::AddRenderLightCommand(Light::Ptr light)
{
    // TODO: All lights can share a single material
    Material::Ptr lightMat = Material::New("Emissive", "glsl_shaders/Emissive.vert", "glsl_shaders/Emissive.frag");
    lightMat->SetCastShadows(false);
    lightMat->AddOrSetVector("uEmissiveColor", light->GetLightColor());

    // Only need to modify the translation column
    glm::mat4 transform = glm::mat4(1.0f);
    glm::vec3 lightPos = light->GetLightPosition();
    transform[3][0] = lightPos.x;
    transform[3][1] = lightPos.y;
    transform[3][2] = lightPos.z;
    m_CommandBuffer->PushCommand(m_LightMesh, lightMat, transform);
}

void SceneRenderGraph::BuildSkyboxRenderCommands()
{
    Material::Ptr skyboxMat = Material::New("Skybox", "glsl_shaders/Cube.vert", "glsl_shaders/Skybox.frag", true);
    skyboxMat->SetCastShadows(false);
    skyboxMat->AddOrSetTextureCube(m_EnvironmentCubemap);
    m_Cube->SetOverrideMaterial(skyboxMat);
    BuildRenderCommands(m_Cube);
}

void SceneRenderGraph::RenderToCubemap(TextureCube::Ptr cubemap, unsigned int mipLevel)
{
    int width = cubemap->GetSize().x >> mipLevel;
    int height = cubemap->GetSize().y >> mipLevel;

    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_CubemapDepthRenderBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_CubemapDepthRenderBufferID);
    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "FrameBuffer is not complete!" << std::endl;
    }
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
    glBindBuffer(GL_UNIFORM_BUFFER, m_GlobalUniformBufferID);

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

    glViewport(0, 0, m_RenderSize.x, m_RenderSize.y);
}

void SceneRenderGraph::DrawSceneNode(SceneNode::Ptr node)
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
        RenderMesh(mr->GetMesh());
    }

    for (size_t i = 0; i < node->GetChildrenCount(); ++i)
    {
        DrawSceneNode(node->GetChildByIndex(i));
    }
}

void SceneRenderGraph::GenerateBRDFLUT()
{
    m_BRDFLUTRT = RenderTarget::New(128, 128, GL_HALF_FLOAT, 1);
    m_BRDFLUTRT->GetColorTexture(0)->SetTextureName("uBRDFLUT");
    Material::Ptr generateBRDFLUTFMat = Material::New("Generate_BRDF_LUT", "glsl_shaders/Blit.vert", "glsl_shaders/GenerateBRDFLUT.frag");
    Blit(nullptr, m_BRDFLUTRT, generateBRDFLUTFMat);
}

void SceneRenderGraph::LoadEnvironment(const std::string &cubemapPath)
{
    // Framebuffer and render buffer for off-screen rendering cubemaps
    glGenFramebuffers(1, &m_FrameBufferID);
    glGenRenderbuffers(1, &m_CubemapDepthRenderBufferID);

    m_Cube = AssetsLoader::LoadModel("models/glTF/Box/glTF-Binary/Box.glb", true);
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

    if (fileExt == "ktx")
    {
        AssetsLoader::InitCubemapKTX(m_EnvironmentCubemap, cubemapPath);
    }
    else if (fileExt == "hdr")
    {
        Texture2D::Ptr environmentMap = AssetsLoader::LoadHDRTexture("uHDRMap", cubemapPath);
        m_EnvironmentCubemap->DefaultInit(environmentMap->GetSize().y, environmentMap->GetSize().y, GL_RGB32F, GL_RGB, GL_FLOAT);

        // Equirectangular map to a cubemap
        Material::Ptr capMat = Material::New("HDR_to_Cubemap", "glsl_shaders/Cube.vert", "glsl_shaders/HDRToCubemap.frag");
        capMat->AddOrSetTexture(environmentMap);
        m_Cube->SetOverrideMaterial(capMat);
        RenderToCubemap(m_EnvironmentCubemap, 0);
    }
    else
    {
        std::cerr << "Unsupport cubemap file, path is: " << cubemapPath << std::endl;
        return;
    }

    // Generate irradiance cubemap and pre-filtered cuebmap
    GenerateCubemaps();
    // Generate environment BRDF look-up table
    GenerateBRDFLUT();
    // Add render the skybox commands
    BuildSkyboxRenderCommands();
}

void SceneRenderGraph::GenerateCubemaps()
{
    // Diffuse irradiance
    m_IrradianceCubemap = TextureCube::New("uIrradianceCubemap");
    m_IrradianceCubemap->DefaultInit(64, 64, GL_RGB32F, GL_RGB, GL_FLOAT);

    Material::Ptr cubemapConvolutionMat = Material::New("Cubemap_Convolution", "glsl_shaders/Cube.vert", "glsl_shaders/IrradianceCubemap.frag");
    cubemapConvolutionMat->AddOrSetTextureCube(m_EnvironmentCubemap);
    m_Cube->SetOverrideMaterial(cubemapConvolutionMat);
    RenderToCubemap(m_IrradianceCubemap, 0);

    // Specular IBL
    m_PrefilteredCubemap = TextureCube::New("uPrefilteredCubemap");
    m_PrefilteredCubemap->DefaultInit(512, 512, GL_RGBA32F, GL_RGB, GL_FLOAT, true);

    Material::Ptr cubemapPrefilteredMat = Material::New("Cubemap_Prefiltered", "glsl_shaders/Cube.vert", "glsl_shaders/PrefilteredCubemap.frag");
    cubemapPrefilteredMat->AddOrSetTextureCube(m_EnvironmentCubemap);
    m_Cube->SetOverrideMaterial(cubemapPrefilteredMat);

    const uint32_t numMips = static_cast<uint32_t>(floor(std::log2(512))) + 1;
    for (unsigned int mip = 0; mip < numMips; ++mip)
    {
        cubemapPrefilteredMat->AddOrSetFloat("uRoughness", (float)(mip) / (numMips - 1));
        RenderToCubemap(m_PrefilteredCubemap, mip);
    }
}

void SceneRenderGraph::BuildRenderCommands(SceneNode::Ptr sceneNode)
{
    glm::mat4 model = sceneNode->GetModelMatrix();
    Material::Ptr overrideMat = sceneNode->OverrideMat;
    for (size_t i = 0; i < sceneNode->MeshRenders.size(); ++i)
        m_CommandBuffer->PushCommand(sceneNode->MeshRenders[i]->GetMesh(), overrideMat ? overrideMat : sceneNode->MeshRenders[i]->GetMaterial(), model);
    
    // Debugging AABB
    if (sceneNode->IsAABBCalculated && false)
        m_CommandBuffer->PushDebuggingCommand(AABBCube::New(sceneNode->AABB.GetCorners()), m_DebuggingAABBMat, model);

    for (size_t i = 0; i < sceneNode->GetChildrenCount(); ++i)
        BuildRenderCommands(sceneNode->GetChildByIndex(i));
}

void SceneRenderGraph::CalculateSceneAABB()
{
    m_Scene->MergeChildrenAABBs(m_Scene->AABB);
    m_Scene->IsAABBCalculated = true;

    // Debugging camera's frustum
//    BoundingFrustum bf;
//    BoundingFrustum::CreateFromMatrix(bf, m_Camera->GetProjectionMatrix());
//    glm::mat4 tr = glm::mat4(1.0f);
//    tr = glm::Translate(tr, m_Camera->GetEyePosition());
//    m_CommandBuffer->PushDebuggingCommand(AABBCube::New(bf.GetCorners()), m_DebuggingAABBMat, tr);

    // Debugging AABB
    // m_CommandBuffer->PushDebuggingCommand(AABBCube::New(m_Scene->AABB.GetCorners()), m_DebuggingAABBMat, glm::mat4(1.0f));
}

void SceneRenderGraph::ComputeShadowProjectionFitViewFrustum(const glm::mat4 &cameraProjection, const glm::mat4 &cameraView, const glm::mat4 &lightView,vec3 &lightCameraOrthographicMin, vec3 &lightCameraOrthographicMax)
{
    glm::mat4 inverseCameraView = glm::inverse(cameraView);

    // Calculate 8 corner points of view frustum
    BoundingFrustum viewFrustum(cameraProjection);
    std::vector<vec3> frustumPoints = viewFrustum.GetCorners();
    
    vec3 tempTranslatedPoint;
    for (size_t i = 0; i < 8; ++i)
    {
        // Transform the frustum from camera view space to world space
        frustumPoints[i] = glm::make_vec3(inverseCameraView * glm::vec4(frustumPoints[i], 1.0f));
        // Transform the frustum from world space to light view space
        tempTranslatedPoint = glm::make_vec3(lightView * glm::vec4(frustumPoints[i], 1.0f));
        // Find the min and max
        lightCameraOrthographicMin = glm::min(tempTranslatedPoint, lightCameraOrthographicMin);
        lightCameraOrthographicMax = glm::max(tempTranslatedPoint, lightCameraOrthographicMax);
    }
}

void SceneRenderGraph::ExecuteCommandBuffer()
{
    Light::Ptr mainLight = m_Lights[0];
    
    Camera::Ptr lightCamera = Camera::New(mainLight->GetLightPosition(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    glm::mat4 viewCameraProjection = m_Camera->GetProjectionMatrix();
    glm::mat4 viewCameraView = m_Camera->GetViewMatrix();

    // Calculate a tight light camera projection to fit the camera view frustum
    vec3 orthographicMin = vec3(FLT_MAX);
    vec3 orthographicMax = vec3(-FLT_MAX);
    ComputeShadowProjectionFitViewFrustum(viewCameraProjection, viewCameraView, lightCamera->GetViewMatrix(), orthographicMin, orthographicMax);

    lightCamera->SetOrthographic(orthographicMin.x, orthographicMax.x, orthographicMin.y, orthographicMax.y, 0.001f, 100.0f);
    glm::mat4 lightCameraVP = lightCamera->GetProjectionMatrix() * lightCamera->GetViewMatrix();

    // Render shadowmap first
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.1f, 4.0f);
    m_ShadowmapRT->Bind();
    m_ShadowCasterMat->Use();
    std::vector<RenderCommand::Ptr> shadowCasterCommands = m_CommandBuffer->GetShadowCasterCommands();
    for (size_t i = 0; i < shadowCasterCommands.size(); ++i)
    {
        RenderCommand::Ptr command = shadowCasterCommands[i];
        m_ShadowCasterMat->SetMatrix("uLightMVP", lightCameraVP * command->Transform);
        RenderMesh(command->Mesh);
    }
    glDisable(GL_POLYGON_OFFSET_FILL);

    // Set global uniforms
    glBindBuffer(GL_UNIFORM_BUFFER, m_GlobalUniformBufferID);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &(viewCameraView[0].x));
    glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &(viewCameraProjection[0].x));
    glBufferSubData(GL_UNIFORM_BUFFER, 128, 16, &(mainLight->GetLightPosition().x));
    glBufferSubData(GL_UNIFORM_BUFFER, 144, 16, &(mainLight->GetLightColor().x));
    glBufferSubData(GL_UNIFORM_BUFFER, 160, 16, &(m_Camera->GetEyePosition().x));
    glBufferSubData(GL_UNIFORM_BUFFER, 176, 64, &(lightCameraVP[0].x));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Bind intermediate framebuffer
    m_IntermediateRT->Bind();

    // Opaque
    std::vector<RenderCommand::Ptr> opaqueCommands = m_CommandBuffer->GetOpaqueCommands();
    for (size_t i = 0; i < opaqueCommands.size(); ++i)
    {
       RenderCommand::Ptr command = opaqueCommands[i];
       RenderCommand(command);
    }

    // Skybox start ----------------
    // Skybox's depth always is 1.0, is equal to the max depth buffer, rendering skybox after opauqe objects and setting depth func to less&equal will
    // ensure that the skybox is only renderered in pixels that are not covered by the opaque objects.
    // Pixels covered by opaque objects have a depth less than 1.0. Therefore, the depth test will never pass when rendering the skybox.
    glDepthFunc(GL_LEQUAL);
    // Depth write off
    glDepthMask(GL_FALSE);
    std::vector<RenderCommand::Ptr> skyboxCommands = m_CommandBuffer->GetSkyboxCommands();
    for (size_t i = 0; i < skyboxCommands.size(); ++i)
    {
        RenderCommand::Ptr command = skyboxCommands[i];
        RenderCommand(command);
    }
    // Depth write on
    glDepthMask(GL_TRUE);
    // Set back to less
    glDepthFunc(GL_LESS);
    // Skybox end ----------------

    // Transparent
    std::vector<RenderCommand::Ptr> transparentCommands = m_CommandBuffer->GetTransparentCommands();
    for (size_t i = 0; i < transparentCommands.size(); ++i)
    {
       RenderCommand::Ptr command = transparentCommands[i];
       RenderCommand(command);
    }

    // Debugging AABB
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    m_DebuggingAABBMat->Use();
    SetGLCull(!m_DebuggingAABBMat->GetDoubleSided());
    std::vector<RenderCommand::Ptr> commands = m_CommandBuffer->GetDebuggingCommands();
    for (size_t i = 0; i < commands.size(); ++i)
    {
        m_DebuggingAABBMat->SetMatrix("uModelMatrix", commands[i]->Transform);
        RenderMesh(commands[i]->Mesh);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    Blit(m_IntermediateRT->GetColorTexture(0), nullptr, m_BlitMat);
//    Blit(m_ShadowmapRT->GetShadowmapTexture(), nullptr, m_BlitMat);
}

void SceneRenderGraph::RenderCommand(RenderCommand::Ptr command)
{
    Mesh::Ptr mesh = command->Mesh;
    Material::Ptr mat = command->Material;

    SetGLCull(!mat->GetDoubleSided());
    SetGLBlend(mat->GetAlphaMode() == Material::AlphaMode::BLEND);

    mat->AddOrSetTextureCube(m_IrradianceCubemap);
    mat->AddOrSetTextureCube(m_PrefilteredCubemap);
    mat->AddOrSetTexture(m_BRDFLUTRT->GetColorTexture(0));

    if (mat->GetMaterialCastShadows())
    {
        mat->AddOrSetTexture(m_ShadowmapRT->GetShadowmapTexture());
    }

    mat->Use();
    mat->SetMatrix("uModelMatrix", command->Transform);
    mat->SetMatrix("uModelMatrixInverse", glm::mat3x3(glm::inverse(command->Transform)));

    RenderMesh(mesh);
}

void SceneRenderGraph::RenderMesh(Mesh::Ptr mesh)
{
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

void SceneRenderGraph::SetGLCull(bool enable)
{
    if (m_CullFace != enable)
    {
        m_CullFace = enable;
        if (enable)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
    }
}

void SceneRenderGraph::SetGLBlend(bool enable)
{
    if (m_Blend != enable)
    {
        m_Blend = enable;
        if (enable)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    }
}

void SceneRenderGraph::Blit(Texture2D::Ptr source, RenderTarget::Ptr destination, Material::Ptr blitMat)
{
    if (destination)
        destination->Bind();
    else
    {
        glViewport(0, 0, m_RenderSize.x, m_RenderSize.y);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    if (blitMat == nullptr)
        blitMat = m_BlitMat;

    if (source)
    {
        source->SetTextureName("uSource");
        blitMat->AddOrSetTexture(source);
    }

    blitMat->Use();

    glBindVertexArray(m_BlitVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}
