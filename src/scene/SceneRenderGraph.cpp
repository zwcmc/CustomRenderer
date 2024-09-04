#include "scene/SceneRenderGraph.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "defines.h"
#include "meshes/AABBCube.h"
#include "utility/Collision.h"
#include "renderer/Blitter.h"
#include "lights/DirectionalLight.h"

using namespace Collision;

SceneRenderGraph::SceneRenderGraph()
    : m_GlobalUniformBufferID(0), m_CullFace(true), m_Blend(false), m_RenderSize(u32vec2(1))
{ }

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
    glBufferData(GL_UNIFORM_BUFFER, 592, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_GlobalUniformBufferID); // Set global uniform to binding point 0
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Initialize Blitter
    Blitter::Init();

    m_IntermediateRT = RenderTarget::New(1, 1, GL_HALF_FLOAT, 1, true);

    // Environment IBL
    m_EnvIBL = EnvironmentIBL::New("textures/environments/papermill.hdr", m_GlobalUniformBufferID);

    // Directional shadow map
    m_DirectionalShadowMap = DirectionalLightShadowMap::New();
    m_DirectionalShadowMap->SetCascadeShadowMapsEnabled(true);
    
    // Post processing
    m_PostProcessing = PostProcessing::New();

    m_DebuggingAABBMat = Material::New("Draw AABB", "utils/DrawBoundingBox.vert", "utils/DrawBoundingBox.frag");
    m_DebuggingAABBMat->SetDoubleSided(true);
}

void SceneRenderGraph::Cleanup()
{
    // Cleanup Blitter
    Blitter::Cleanup();
    
    glDeleteBuffers(1, &m_GlobalUniformBufferID);
}

void SceneRenderGraph::SetRenderSize(const int &width, const int &height)
{
    m_RenderSize.x = width;
    m_RenderSize.y = height;

    m_Camera->SetScreenSize(width, height);

    m_IntermediateRT->SetSize(u32vec2(width, height));
}

void SceneRenderGraph::SetCamera(Camera::Ptr camera)
{
    m_Camera = camera;
}

void SceneRenderGraph::SetMainLight(DirectionalLight::Ptr light)
{
    m_MainLight = light;
}

void SceneRenderGraph::AddSceneNode(SceneNode::Ptr sceneNode)
{
    m_Scene->AddChild(sceneNode);
}

void SceneRenderGraph::BuildSkyboxRenderCommands()
{
    BuildRenderCommands(m_EnvIBL->GetSkyboxRenderNode());
}

void SceneRenderGraph::BuildRenderCommands(SceneNode::Ptr sceneNode)
{
    mat4 model = sceneNode->GetModelMatrix();
    Material::Ptr overrideMat = sceneNode->OverrideMat;
    for (size_t i = 0; i < sceneNode->MeshRenders.size(); ++i)
    {
        m_CommandBuffer->PushCommand(sceneNode->MeshRenders[i]->GetMesh(), overrideMat ? overrideMat : sceneNode->MeshRenders[i]->GetMaterial(), model);
    }

    // Debugging render node AABB
    if (sceneNode->IsAABBCalculated && false)
    {
        m_CommandBuffer->PushDebuggingCommand(AABBCube::New(sceneNode->AABB.GetCorners()), m_DebuggingAABBMat, model);
    }

    for (size_t i = 0; i < sceneNode->GetChildrenCount(); ++i)
    {
        BuildRenderCommands(sceneNode->GetChildByIndex(i));
    }
}

void SceneRenderGraph::CalculateSceneAABB()
{
    bool firstMerge = true;
    m_Scene->MergeChildrenAABBs(m_Scene->AABB, firstMerge);

    // Debugging camera's frustum
//    BoundingFrustum bf;
//    BoundingFrustum::CreateFromMatrix(bf, m_Camera->GetProjectionMatrix());
//    mat4 tr = mat4(1.0f);
//    tr = translate(tr, m_Camera->GetEyePosition());
//    m_CommandBuffer->PushDebuggingCommand(AABBCube::New(bf.GetCorners()), m_DebuggingAABBMat, tr);

    // Debugging scene AABB
//    m_CommandBuffer->PushDebuggingCommand(AABBCube::New(m_Scene->AABB.GetCorners()), m_DebuggingAABBMat);
}

void SceneRenderGraph::PepareRenderCommands()
{
    m_CommandBuffer->Clear();
    
    // Calculate the scene AABB
    CalculateSceneAABB();

    // Build scene render commands
    BuildRenderCommands(m_Scene);
    
    // Build skybox render commands
    BuildSkyboxRenderCommands();
}

void SceneRenderGraph::Render()
{
    // Build render commands
    PepareRenderCommands();

    DirectionalLight::Ptr currentLight = m_MainLight;
    Camera::Ptr currentCamera = m_Camera;

    // Render shadow map first
    if (currentLight->IsCastShadow())
    {
        m_DirectionalShadowMap->RenderShadowMap(currentCamera, currentLight, m_CommandBuffer->GetShadowCasterCommands(), m_Scene);
    }

    // Set global uniforms
    glBindBuffer(GL_UNIFORM_BUFFER, m_GlobalUniformBufferID);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &(currentCamera->GetViewMatrix()[0].x));
    glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &(currentCamera->GetProjectionMatrix()[0].x));
    glBufferSubData(GL_UNIFORM_BUFFER, 128, 16, &(currentLight->GetLightPosition().x));
    glBufferSubData(GL_UNIFORM_BUFFER, 144, 16, &(currentLight->GetLightColor().x));
    glBufferSubData(GL_UNIFORM_BUFFER, 160, 16, &(currentCamera->GetEyePosition().x));

    // Set light cascade data
    if (currentLight->IsCastShadow())
    {
        glBufferSubData(GL_UNIFORM_BUFFER, 176, 256, &(m_DirectionalShadowMap->GetShadowProjections()[0][0].x));
        glBufferSubData(GL_UNIFORM_BUFFER, 432, 64, &(m_DirectionalShadowMap->GetLightCameraView()[0].x));
        glBufferSubData(GL_UNIFORM_BUFFER, 496, 64, &(m_DirectionalShadowMap->GetCascadeScalesAndOffsets()[0].x));
        glBufferSubData(GL_UNIFORM_BUFFER, 560, 16, &(m_DirectionalShadowMap->GetShadowCascadeParams()));
    }

    glm::u32vec2 shadowMapSize = currentLight->GetShadowMapSize();
    glm::vec4 shadowMapTexelSize = glm::vec4(1.0f / shadowMapSize.x, 1.0f / shadowMapSize.y, shadowMapSize.x, shadowMapSize.y);
    glBufferSubData(GL_UNIFORM_BUFFER, 576, 16, &shadowMapTexelSize.x);

    // Set float[4];
    // glBufferSubData(GL_UNIFORM_BUFFER, 496, 64, &cascadeScales); does not work?
    // for (size_t i = 0; i < 4; ++i)
    //     glBufferSubData(GL_UNIFORM_BUFFER, 496 + (i * 16), 16, &cascadeScales[i]);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Blitter::BlitToCameraTarget(currentLight->GetShadowMapRT()->GetShadowMapTexture(), currentCamera); return;

    // Bind intermediate framebuffer
    m_IntermediateRT->Bind();

    // Opaque
    std::vector<RenderCommand::Ptr> opaqueCommands = m_CommandBuffer->GetOpaqueCommands();
    for (size_t i = 0; i < opaqueCommands.size(); ++i)
    {
       RenderCommand::Ptr command = opaqueCommands[i];
       RenderCommand(command, currentLight);
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
        RenderCommand(command, currentLight);
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
       RenderCommand(command, currentLight);
    }

    // Debugging AABB
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    m_DebuggingAABBMat->Use();
    SetGLCull(!m_DebuggingAABBMat->GetDoubleSided());
    std::vector<RenderCommand::Ptr> commands = m_CommandBuffer->GetDebuggingCommands();
    for (size_t i = 0; i < commands.size(); ++i)
    {
        m_DebuggingAABBMat->SetMatrix("uModelToWorld", commands[i]->Transform);
        RenderMesh(commands[i]->Mesh);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    m_PostProcessing->Render(m_IntermediateRT, currentCamera);
}

void SceneRenderGraph::RenderCommand(RenderCommand::Ptr command, Light::Ptr light)
{
    Mesh::Ptr mesh = command->Mesh;
    Material::Ptr mat = command->Material;

    SetGLCull(!mat->GetDoubleSided());
    SetGLBlend(mat->GetAlphaMode() == Material::AlphaMode::BLEND);

    if (!mat->IsUsedForSkybox())
    {
        mat->AddOrSetTextureCube(m_EnvIBL->GetIrradiance());
        mat->AddOrSetTextureCube(m_EnvIBL->GetPrefiltered());
        mat->AddOrSetTexture(m_EnvIBL->GetBRDFLUTTexture());
    }

    if (mat->GetMaterialCastShadows())
    {
        if (light->IsCastShadow())
        {
            mat->AddOrSetFloat("uShadowMapSet", 1.0f);
            mat->AddOrSetTexture(light->GetShadowMapRT()->GetShadowMapTexture());
        }
        else
        {
            mat->AddOrSetFloat("uShadowMapSet", -1.0f);
            mat->AddOrSetTexture(light->GetEmptyShadowMapTexture());
        }
    }

    mat->Use();
    
    if (!mat->IsUsedForSkybox())
    {
        mat->SetMatrix("uModelToWorld", command->Transform);
        mat->SetMatrix("uModelNormalToWorld", FastCofactor(mat3x3(command->Transform)));
    }

    RenderMesh(mesh);
}

glm::mat3 SceneRenderGraph::FastCofactor(const glm::mat3 &m)
{
    // Assuming the input matrix is:
    // | a b c |
    // | d e f |
    // | g h i |
    //
    // The cofactor are
    // | A B C |
    // | D E F |
    // | G H I |

    // Where:
    // A = (ei - fh), B = (fg - di), C = (dh - eg)
    // D = (ch - bi), E = (ai - cg), F = (bg - ah)
    // G = (bf - ce), H = (cd - af), I = (ae - bd)

    // Importantly, matrices are column-major!

    glm::mat3 cof;

    const float a = m[0][0];
    const float b = m[1][0];
    const float c = m[2][0];
    const float d = m[0][1];
    const float e = m[1][1];
    const float f = m[2][1];
    const float g = m[0][2];
    const float h = m[1][2];
    const float i = m[2][2];

    cof[0][0] = e * i - f * h; // A
    cof[0][1] = c * h - b * i; // D
    cof[0][2] = b * f - c * e; // G
    cof[1][0] = f * g - d * i; // B
    cof[1][1] = a * i - c * g; // E
    cof[1][2] = c * d - a * f; // H
    cof[2][0] = d * h - e * g; // C
    cof[2][1] = b * g - a * h; // F
    cof[2][2] = a * e - b * d; // I

    return cof;
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
