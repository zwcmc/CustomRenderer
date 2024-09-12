#include "scene/SceneRenderGraph.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "defines.h"
#include "meshes/AABBCube.h"
#include "utility/Collision.h"
#include "renderer/Blitter.h"
#include "lights/DirectionalLight.h"

#include "utility/StatusRecorder.h"

using namespace Collision;

SceneRenderGraph::SceneRenderGraph()
    : m_GlobalUniformBufferID(0), m_RenderSize(u32vec2(1))
{ }

void SceneRenderGraph::Init()
{
    m_Scene = SceneNode::New();

    m_CommandBuffer = CommandBuffer::New();
    
    // OpenGL state
    m_GLStateCache = GLStateCache::New();
    m_GLStateCache->InitState();

    // Global uniform buffer object
    glGenBuffers(1, &m_GlobalUniformBufferID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_GlobalUniformBufferID);
    glBufferData(GL_UNIFORM_BUFFER, 672, nullptr, GL_STATIC_DRAW);
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
    
    // Deffered rendering gbuffer
    m_GBufferRT = RenderTarget::New(1, 1, GL_HALF_FLOAT, 4, true);
    m_DeferredLightingMat = Material::New("Deferred Lighting", "utils/FullScreenTriangle.vs", "DeferredLit.fs");
    
    // SSAO
    m_SSAO = SSAO::New();

    m_DebuggingAABBMat = Material::New("Draw AABB", "utils/DrawBoundingBox.vs", "utils/DrawBoundingBox.fs");
    m_DebuggingAABBMat->SetRenderFace(Material::RenderFace::BOTH);
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

    m_IntermediateRT->SetSize(glm::u32vec2(width, height));
    
    m_GBufferRT->SetSize(width, height);
    
    m_SSAO->SetRenderTargetSize(width, height);
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

//    // Debugging camera's frustum
//    BoundingFrustum bf;
//    BoundingFrustum::CreateFromMatrix(bf, m_Camera->GetProjectionMatrix());
//    mat4 tr = mat4(1.0f);
//    tr = translate(tr, m_Camera->GetEyePosition());
//    m_CommandBuffer->PushDebuggingCommand(AABBCube::New(bf.GetCorners()), m_DebuggingAABBMat, tr);

//    // Debugging scene AABB
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

void SceneRenderGraph::UpdateGlobalUniformsData(const Camera::Ptr camera, const Light::Ptr light)
{
    // Set global uniforms
    glBindBuffer(GL_UNIFORM_BUFFER, m_GlobalUniformBufferID);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &(camera->GetViewMatrix()[0].x));
    glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &(camera->GetProjectionMatrix()[0].x));
    glBufferSubData(GL_UNIFORM_BUFFER, 128, 16, &(light->GetLightPosition().x));
    glBufferSubData(GL_UNIFORM_BUFFER, 144, 16, &(light->GetLightColor().x));
    glBufferSubData(GL_UNIFORM_BUFFER, 160, 16, &(camera->GetEyePosition().x));

    // Set light cascade data
    if (light->IsCastShadow())
    {
        glBufferSubData(GL_UNIFORM_BUFFER, 176, 256, &(m_DirectionalShadowMap->GetShadowProjections()[0][0].x));
        glBufferSubData(GL_UNIFORM_BUFFER, 432, 64, &(m_DirectionalShadowMap->GetLightCameraView()[0].x));
        glBufferSubData(GL_UNIFORM_BUFFER, 496, 64, &(m_DirectionalShadowMap->GetCascadeScalesAndOffsets()[0].x));
        glBufferSubData(GL_UNIFORM_BUFFER, 560, 16, &(m_DirectionalShadowMap->GetShadowCascadeParams()));
    }

    glm::u32vec2 shadowMapSize = light->GetShadowMapSize();
    glm::vec4 shadowMapTexelSize = glm::vec4(1.0f / shadowMapSize.x, 1.0f / shadowMapSize.y, shadowMapSize.x, shadowMapSize.y);
    glBufferSubData(GL_UNIFORM_BUFFER, 576, 16, &shadowMapTexelSize.x);
    
    glBufferSubData(GL_UNIFORM_BUFFER, 592, 64, &(glm::inverse(camera->GetProjectionMatrix())[0].x));
    glm::vec4 zBufferParams = glm::vec4(1.0f / camera->GetFar(), 0.0f, 0.0f, 0.0f);
    glBufferSubData(GL_UNIFORM_BUFFER, 656, 16, &zBufferParams.x);

    // Set float[4];
    // glBufferSubData(GL_UNIFORM_BUFFER, 496, 64, &cascadeScales); does not work?
    // for (size_t i = 0; i < 4; ++i)
    //     glBufferSubData(GL_UNIFORM_BUFFER, 496 + (i * 16), 16, &cascadeScales[i]);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void SceneRenderGraph::Render()
{
    // Build render commands
    PepareRenderCommands();
    
    m_GLStateCache->SetDepthTest(true);
    m_GLStateCache->SetDepthFunc(GL_LESS);
    m_GLStateCache->SetDepthWriteMask(GL_TRUE);

    DirectionalLight::Ptr currentLight = m_MainLight;
    Camera::Ptr currentCamera = m_Camera;

    // Render shadow map
    if (currentLight->IsCastShadow())
    {
        m_DirectionalShadowMap->RenderShadowMap(currentCamera, currentLight, m_CommandBuffer->GetShadowCasterCommands(), m_Scene);
    }

    UpdateGlobalUniformsData(currentCamera, currentLight);
    
    bool isDeferred = StatusRecorder::DeferredRendering;
    if (isDeferred)
    {
        m_GBufferRT->BindTarget(true, true);

        unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(4, attachments);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Opaque
        std::vector<RenderCommand::Ptr> opaqueCommands = m_CommandBuffer->GetOpaqueCommands();
        for (size_t i = 0; i < opaqueCommands.size(); ++i)
        {
            RenderCommand::Ptr command = opaqueCommands[i];
            RenderCommand(command, currentLight);
        }

        attachments[1] = GL_NONE;
        attachments[2] = GL_NONE;
        attachments[3] = GL_NONE;
        glDrawBuffers(4, attachments);
        
        if (StatusRecorder::SSAO)
        {
            // SSAO
            // Copy depth to ssao render target for depth testing to skip pixels at max 1.0 (i.e. the skybox)
            m_GLStateCache->SetDepthFunc(GL_ALWAYS);
            m_SSAO->CopyDepth(m_GBufferRT);
            m_GLStateCache->SetDepthFunc(GL_NOTEQUAL);
            m_SSAO->Render(m_GBufferRT);
//            Blitter::BlitCamera(m_SSAO->GetSAO(), currentCamera); return;
        }

        m_GLStateCache->SetDepthFunc(GL_ALWAYS);
        // Deferred lighting
        m_GLStateCache->SetDepthTest(false);

        m_DeferredLightingMat->AddOrSetTexture("uGBuffer0", m_GBufferRT->GetColorTexture(0));
        m_DeferredLightingMat->AddOrSetTexture("uGBuffer1", m_GBufferRT->GetColorTexture(1));
        m_DeferredLightingMat->AddOrSetTexture("uGBuffer2", m_GBufferRT->GetColorTexture(2));
        m_DeferredLightingMat->AddOrSetTexture("uGBuffer3", m_GBufferRT->GetColorTexture(3));

        SetMatIBLAndShadow(m_DeferredLightingMat, currentLight);
        
        if (StatusRecorder::SSAO)
        {
            m_DeferredLightingMat->AddOrSetFloat("uSSAOSet", 1.0f);
            m_DeferredLightingMat->AddOrSetTexture("uSSAOTexture", m_SSAO->GetSAO());
        }
        else
        {
            m_DeferredLightingMat->AddOrSetFloat("uSSAOSet", -1.0f);
        }

        Blitter::RenderToTarget(m_IntermediateRT, m_DeferredLightingMat);
        
        m_GLStateCache->SetDepthTest(true);

        // Copy depth
        Blitter::CopyDepth(m_GBufferRT, m_IntermediateRT);

        // Bind intermediate framebuffer
        m_IntermediateRT->BindTarget(false, false);
    }
    else
    {
        // Bind intermediate framebuffer
        m_IntermediateRT->BindTarget(true, true);

        // Opaque
        std::vector<RenderCommand::Ptr> opaqueCommands = m_CommandBuffer->GetOpaqueCommands();
        for (size_t i = 0; i < opaqueCommands.size(); ++i)
        {
           RenderCommand::Ptr command = opaqueCommands[i];
           RenderCommand(command, currentLight);
        }
    }

    // Blitter::BlitCamera(m_IntermediateRT->GetDepthTexture(), currentCamera); return;

    // Skybox start ----------------
    // Skybox's depth always is 1.0, is equal to the max depth buffer, rendering skybox after opauqe objects and setting depth func to less&equal will
    // ensure that the skybox is only renderered in pixels that are not covered by the opaque objects.
    // Pixels covered by opaque objects have a depth less than 1.0. Therefore, the depth test will never pass when rendering the skybox.
    m_GLStateCache->SetDepthWriteMask(GL_FALSE);
    m_GLStateCache->SetDepthFunc(GL_LEQUAL);
    std::vector<RenderCommand::Ptr> skyboxCommands = m_CommandBuffer->GetSkyboxCommands();
    for (size_t i = 0; i < skyboxCommands.size(); ++i)
    {
        RenderCommand::Ptr command = skyboxCommands[i];
        RenderCommand(command, currentLight);
    }
    m_GLStateCache->SetDepthWriteMask(GL_TRUE);
    m_GLStateCache->SetDepthFunc(GL_LESS);
    // Skybox end ----------------

    // Debugging AABB
    m_GLStateCache->SetPolygonMode(GL_LINE);
    std::vector<RenderCommand::Ptr> debuggingCommands = m_CommandBuffer->GetDebuggingCommands();
    for (size_t i = 0; i < debuggingCommands.size(); ++i)
    {
        RenderCommand::Ptr command = debuggingCommands[i];
        RenderCommand(command, currentLight);
    }
    m_GLStateCache->SetPolygonMode(GL_FILL);

    m_GLStateCache->SetDepthTest(false);
    // Transparent
    std::vector<RenderCommand::Ptr> transparentCommands = m_CommandBuffer->GetTransparentCommands();
    for (size_t i = 0; i < transparentCommands.size(); ++i)
    {
       RenderCommand::Ptr command = transparentCommands[i];
       RenderCommand(command, currentLight);
    }

    m_PostProcessing->Render(m_IntermediateRT, currentCamera);
}

void SceneRenderGraph::RenderCommand(RenderCommand::Ptr command, Light::Ptr light)
{
    Mesh::Ptr mesh = command->Mesh;
    Material::Ptr mat = command->Material;

    Material::RenderFace face = mat->GetRenderFace();
    if (face == Material::RenderFace::BOTH)
    {
        m_GLStateCache->SetCull(false);
    }
    else
    {
        m_GLStateCache->SetCull(true);
        m_GLStateCache->SetCullFace(face == Material::RenderFace::FRONT ? GL_BACK : GL_FRONT);
    }
    Material::AlphaMode alphaMode = mat->GetAlphaMode();
    if (alphaMode == Material::AlphaMode::BLEND)
    {
        m_GLStateCache->SetBlend(true);
        m_GLStateCache->SetBlendFactor(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        m_GLStateCache->SetBlend(false);
        m_GLStateCache->SetBlendFactor(GL_ONE, GL_ZERO);
    }

    SetMatIBLAndShadow(mat, light);

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

void SceneRenderGraph::SetMatIBLAndShadow(Material::Ptr &mat, Light::Ptr light)
{
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
