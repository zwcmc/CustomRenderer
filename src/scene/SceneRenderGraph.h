#pragma once

#include <iostream>
#include <vector>

#include <glad/glad.h>

#include "ptr.h"

#include "cameras/Camera.h"
#include "lights/Light.h"

#include "meshes/Sphere.h"

#include "renderer/RenderCommand.h"
#include "renderer/CommandBuffer.h"
#include "renderer/RenderTarget.h"

#include "scene/SceneNode.h"

#include "renderer/EnvironmentIBL.h"

class SceneRenderGraph
{
    SHARED_PTR(SceneRenderGraph)
public:
    SceneRenderGraph();
    ~SceneRenderGraph();

    void SetRenderSize(const int &width, const int &height);
    void SetCamera(Camera::Ptr camera);
    Camera::Ptr GetActiveCamera() { return m_Camera; }

    void AddLight(Light::Ptr light);

    void Init();
    void Cleanup();

    void AddSceneNode(SceneNode::Ptr sceneNode);
    void ExecuteCommandBuffer();
    void RenderCommand(RenderCommand::Ptr command);
    void RenderMesh(Mesh::Ptr mesh);
    
    void CalculateSceneAABB();

private:

    void AddRenderLightCommand(Light::Ptr light);
    void BuildSkyboxRenderCommands();
    void BuildRenderCommands(SceneNode::Ptr sceneNode);

    void ComputeShadowProjectionFitViewFrustum(std::vector<vec3> &frustumPoints, const glm::mat4 &cameraView, const glm::mat4 &lightView, vec3 &lightCameraOrthographicMin, vec3 &lightCameraOrthographicMax);
    void RemoveShimmeringEdgeEffect(const std::vector<vec3> &frustumPoints, const glm::u32vec2 &bufferSize, vec3 &lightCameraOrthographicMin, vec3 &lightCameraOrthographicMax);

    void SetGLCull(bool enable);
    void SetGLBlend(bool enable);

    bool m_CullFace;
    bool m_Blend;

    glm::u32vec2 m_RenderSize;

    SceneNode::Ptr m_Scene;

    CommandBuffer::Ptr m_CommandBuffer;
    Camera::Ptr m_Camera;
    std::vector<Light::Ptr> m_Lights;

    GLuint m_GlobalUniformBufferID;

    Sphere::Ptr m_LightMesh;

    RenderTarget::Ptr m_IntermediateRT;

    // Environment IBL
    EnvironmentIBL::Ptr m_EnvIBL;

    // Main light shadowmap
    Material::Ptr m_ShadowCasterMat;
    RenderTarget::Ptr m_ShadowmapRT;

    // Should match GlobalUniforms in Uniforms.glsl
    // struct GlobalUniforms
    // {
    //     glm::mat4 view;              // 64 bytes;  byte offset = 0;
    //     glm::mat4 projection;        // 64 bytes;  byte offset = 64;
    //     vec3 lightDirection0;        // 16 bytes;  byte offset = 128;
    //     vec3 lightColor0;            // 16 bytes;  byte offset = 144;
    //     vec3 cameraPos;              // 16 bytes;  byte offset = 160;
    //     glm::mat4 worldToMainLight;  // 64 bytes;  byte offset = 176;
    // };                               // total bytes = 240

    Material::Ptr m_DebuggingAABBMat;
};
