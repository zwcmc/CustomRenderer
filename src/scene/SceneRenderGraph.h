#pragma once

#include <iostream>
#include <vector>

#include <glad/glad.h>

#include "ptr.h"

#include "cameras/Camera.h"
#include "lights/DirectionalLight.h"

#include "renderer/RenderCommand.h"
#include "renderer/CommandBuffer.h"
#include "renderer/RenderTarget.h"

#include "scene/SceneNode.h"

#include "renderer/EnvironmentIBL.h"
#include "renderer/DirectionalLightShadowMap.h"
#include "renderer/PostProcessing.h"

#include "renderer/GLStateCache.h"

#include "renderer/ScreenSpaceAmbientOcclusion.h"

using namespace glm;

class SceneRenderGraph
{
    SHARED_PTR(SceneRenderGraph)
public:
    SceneRenderGraph();
    ~SceneRenderGraph() = default;

    void SetRenderSize(const int &width, const int &height);
    void SetCamera(Camera::Ptr camera);
    Camera::Ptr GetActiveCamera() { return m_Camera; }

    void SetMainLight(DirectionalLight::Ptr light);

    void Init();
    void Cleanup();

    void AddSceneNode(SceneNode::Ptr sceneNode);

    void Render();
    
    void RenderCommand(RenderCommand::Ptr command, Light::Ptr light);
    void RenderMesh(Mesh::Ptr mesh);
    void CalculateSceneAABB();

private:

    void PepareRenderCommands();
    void UpdateGlobalUniformsData(const Camera::Ptr camera, const Light::Ptr light);

    void BuildSkyboxRenderCommands();
    void BuildRenderCommands(SceneNode::Ptr sceneNode);

    glm::mat3 FastCofactor(const glm::mat3 &matrix);

    void SetMatIBLAndShadow(Material::Ptr &mat, Light::Ptr light);

    // OpenGL state cache
    GLStateCache::Ptr m_GLStateCache;

    u32vec2 m_RenderSize;

    SceneNode::Ptr m_Scene;

    CommandBuffer::Ptr m_CommandBuffer;
    Camera::Ptr m_Camera;
    DirectionalLight::Ptr m_MainLight;

    GLuint m_GlobalUniformBufferID;

    RenderTarget::Ptr m_IntermediateRT;

    // Environment IBL
    EnvironmentIBL::Ptr m_EnvIBL;

    // Directional shadow map
    DirectionalLightShadowMap::Ptr m_DirectionalShadowMap;

    // Post processing
    PostProcessing::Ptr m_PostProcessing;

    // Deferred rendering gbuffer
    RenderTarget::Ptr m_GBufferRT;
    Material::Ptr m_DeferredLightingMat;

    // SSAO
    ScreenSpaceAmbientOcclusion::Ptr m_ScreenSpaceAmbientOcclusion;

    // m_GlobalUniformBufferID
    // Should match GlobalUniforms in Uniforms.glsl
    // struct GlobalUniforms
    // {
    //     mat4 ViewFromWorld;                    // 64 bytes;   byte offset = 0;
    //     mat4 ClipFromView;                     // 64 bytes;   byte offset = 64;
    //     vec3 MainLightPosition;                // 16 bytes;   byte offset = 128;
    //     vec3 MainLightColor;                   // 16 bytes;   byte offset = 144;
    //     vec3 CameraPosition;                   // 16 bytes;   byte offset = 160;
    //     mat4 ShadowClipFromView[4];            // 256 bytes;  byte offset = 176;
    //     mat4 ShadowViewFromWorld;              // 64 bytes;   byte offset = 432;
    //     vec4 CascadeScalesAndOffsets[4];       // 64 bytes;   byte offset = 496;
    //     vec4 CascadeParams;                    // 16 bytes;   byte offset = 560;
    //     vec4 ShadowMapTexelSize;               // 16 bytes;   byte offset = 576;
    //     mat4 ViewFromClip;                     // 64 bytes;   byte offset = 592;
    //     vec4 ZBufferParams;                    // 16 bytes;   byte offset = 656;
    // };                                         // Total bytes = 672

    Material::Ptr m_DebuggingAABBMat;
};
