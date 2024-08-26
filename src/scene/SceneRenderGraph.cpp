#include "scene/SceneRenderGraph.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "defines.h"
#include "meshes/AABBCube.h"
#include "utility/Collision.h"
#include "renderer/Blitter.h"

using namespace Collision;

SceneRenderGraph::SceneRenderGraph()
    : m_GlobalUniformBufferID(0), m_CullFace(true), m_Blend(false), m_RenderSize(glm::u32vec2(1))
{ }

SceneRenderGraph::~SceneRenderGraph()
{
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

    // Init Blitter
    Blitter::Init();

    m_IntermediateRT = RenderTarget::New(1, 1, GL_HALF_FLOAT, 1, true);

    // Environment IBL
    m_EnvIBL = EnvironmentIBL::New("textures/environments/ktx/papermill.ktx", m_GlobalUniformBufferID);
    // Add render the skybox commands
    BuildSkyboxRenderCommands();

    // Main light shadowmap
    m_ShadowmapRT = RenderTarget::New(2048, 2048, GL_FLOAT, 0, false, true);
    m_ShadowCasterMat = Material::New("ShadowCaster", "glsl_shaders/ShadowCaster.vert", "glsl_shaders/ShadowCaster.frag");

    m_DebuggingAABBMat = Material::New("Draw AABB", "glsl_shaders/utils/DrawBoundingBox.vert", "glsl_shaders/utils/DrawBoundingBox.frag");
    m_DebuggingAABBMat->SetDoubleSided(true);
}

void SceneRenderGraph::Cleanup()
{
    // Cleanup Blitter
    Blitter::Cleanup();
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
    BuildRenderCommands(m_EnvIBL->GetSkyboxRenderNode());
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

void SceneRenderGraph::ComputeShadowProjectionFitViewFrustum(std::vector<vec3> &frustumPoints, const glm::mat4 &cameraView, const glm::mat4 &lightView,vec3 &lightCameraOrthographicMin, vec3 &lightCameraOrthographicMax)
{
    glm::mat4 inverseCameraView = glm::inverse(cameraView);
    
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

void SceneRenderGraph::RemoveShimmeringEdgeEffect(const std::vector<vec3> &frustumPoints, const glm::u32vec2 &bufferSize, vec3 &lightCameraOrthographicMin, vec3 &lightCameraOrthographicMax)
{
    vec3 vWorldUnitsPerTexel = vec3(0.0f);
    
    // Fit to the scene
    vec3 vDiagonal = frustumPoints[0] - frustumPoints[6];
    float fCascadeBound = glm::length(vDiagonal);
    vec3 vBoarderOffset = (vec3(fCascadeBound) - (lightCameraOrthographicMax - lightCameraOrthographicMin)) * 0.5f;
    vBoarderOffset.z = 0.0f;
    lightCameraOrthographicMax += vBoarderOffset;
    lightCameraOrthographicMin -= vBoarderOffset;
    // The world units per texel are used to snap  the orthographic projection to texel sized increments.
    vWorldUnitsPerTexel = vec3(fCascadeBound / bufferSize.x, fCascadeBound / bufferSize.y, 0.0);
    
//    // Fit to the cascade
//    int iPCFBlurSize = 2;
//    float fScaleDuetoBlureAMT = (float)(iPCFBlurSize * 2 + 1) / bufferSize.x;
//    vec3 normalizeByBufferSize = vec3(1.0f / bufferSize.x, 1.0f / bufferSize.y, 0.0f);
//    vec3 boardOffset = lightCameraOrthographicMax - lightCameraOrthographicMin;
//    boardOffset *= 0.5f;
//    boardOffset *= fScaleDuetoBlureAMT;
//    lightCameraOrthographicMax += boardOffset;
//    lightCameraOrthographicMin -= boardOffset;
//    // The world units per texel are used to snap  the orthographic projection to texel sized increments.
//    vWorldUnitsPerTexel = lightCameraOrthographicMax - lightCameraOrthographicMin;
//    vWorldUnitsPerTexel *= normalizeByBufferSize;
    
    lightCameraOrthographicMin /= vWorldUnitsPerTexel;
    lightCameraOrthographicMin = floor(lightCameraOrthographicMin);
    lightCameraOrthographicMin *= vWorldUnitsPerTexel;
    
    lightCameraOrthographicMax /= vWorldUnitsPerTexel;
    lightCameraOrthographicMax = floor(lightCameraOrthographicMax);
    lightCameraOrthographicMax *= vWorldUnitsPerTexel;
}

void SceneRenderGraph::ExecuteCommandBuffer()
{
    Light::Ptr mainLight = m_Lights[0];
    
    Camera::Ptr lightCamera = Camera::New(mainLight->GetLightPosition(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    glm::mat4 viewCameraProjection = m_Camera->GetProjectionMatrix();
    glm::mat4 viewCameraView = m_Camera->GetViewMatrix();

    // Calculate a tight light camera projection to fit the camera view frustum

    // Calculate 8 corner points of view frustum
    BoundingFrustum viewFrustum(viewCameraProjection);
    std::vector<vec3> frustumPoints = viewFrustum.GetCorners();
    vec3 vLightCameraOrthographicMin = vec3(FLT_MAX);
    vec3 vLightCameraOrthographicMax = vec3(-FLT_MAX);
    ComputeShadowProjectionFitViewFrustum(frustumPoints, viewCameraView, lightCamera->GetViewMatrix(), vLightCameraOrthographicMin, vLightCameraOrthographicMax);
    
    // Remove the shimmering edge effect along the edges of shadows due to the light changing to fit the camera by moving the light in texel-sized increments
    glm::u32vec2 shadowmapSize = m_ShadowmapRT->GetSize();
    RemoveShimmeringEdgeEffect(frustumPoints, shadowmapSize, vLightCameraOrthographicMin, vLightCameraOrthographicMax);

    // Create the tight orthographic projection for the light camera
    lightCamera->SetOrthographic(vLightCameraOrthographicMin.x, vLightCameraOrthographicMax.x, vLightCameraOrthographicMin.y, vLightCameraOrthographicMax.y, 0.001f, 100.0f);
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

    Blitter::BlitToCamera(m_IntermediateRT->GetColorTexture(0), m_RenderSize);
    // Blitter::BlitToCamera(m_ShadowmapRT->GetShadowmapTexture(), m_RenderSize);
}

void SceneRenderGraph::RenderCommand(RenderCommand::Ptr command)
{
    Mesh::Ptr mesh = command->Mesh;
    Material::Ptr mat = command->Material;

    SetGLCull(!mat->GetDoubleSided());
    SetGLBlend(mat->GetAlphaMode() == Material::AlphaMode::BLEND);

    mat->AddOrSetTextureCube(m_EnvIBL->GetIrradiance());
    mat->AddOrSetTextureCube(m_EnvIBL->GetPrefiltered());
    mat->AddOrSetTexture(m_EnvIBL->GetBRDFLUTTexture());

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
