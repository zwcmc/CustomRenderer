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

    void BuildSkyboxRenderCommands();
    void BuildRenderCommands(SceneNode::Ptr sceneNode);

    void SetGLCull(bool enable);
    void SetGLBlend(bool enable);

    bool m_CullFace;
    bool m_Blend;

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

    // Should match GlobalUniforms in Uniforms.glsl
    // struct GlobalUniforms
    // {
    //     mat4 ViewMatrix;                       // 64 bytes;   byte offset = 0;
    //     mat4 ProjectionMatrix;                 // 64 bytes;   byte offset = 64;
    //     vec3 MainLightDirection;               // 16 bytes;   byte offset = 128;
    //     vec3 MainLightColor;                   // 16 bytes;   byte offset = 144;
    //     vec3 CameraPosition;                   // 16 bytes;   byte offset = 160;
    //     mat4 ShadowProjections[4];             // 256 bytes;  byte offset = 176;
    //     mat4 ShadowView;                       // 64 bytes;   byte offset = 432;
    //     vec4 CascadeScalesAndOffsets[4];       // 64 bytes;   byte offset = 496;
    //     vec4 CascadeParams;                    // 16 bytes;   byte offset = 560;
    //     vec4 ShadowMapTexelSize;               // 16 bytes;    byte offset = 576;
    // };                                         // Total bytes = 592

    Material::Ptr m_DebuggingAABBMat;
};
