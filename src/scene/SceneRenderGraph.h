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

using namespace glm;

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

    void ComputeShadowProjectionFitViewFrustum(std::vector<vec3> &frustumPoints, const mat4 &cameraView, const mat4 &lightView, vec3 &lightCameraOrthographicMin, vec3 &lightCameraOrthographicMax);
    void RemoveShimmeringEdgeEffect(const std::vector<vec3> &frustumPoints, const u32vec2 &bufferSize, vec3 &lightCameraOrthographicMin, vec3 &lightCameraOrthographicMax);
    void ComputeNearAndFar(float &nearPlane, float &farPlane, const vec3 &lightCameraOrthographicMin, const vec3 &lightCameraOrthographicMax, const std::vector<vec3> &sceneAABBPointsLightSpace);

    void SetGLCull(bool enable);
    void SetGLBlend(bool enable);

    bool m_CullFace;
    bool m_Blend;

    u32vec2 m_RenderSize;

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
    RenderTarget::Ptr m_ShadowMapRT;

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
    // };                                         // Total bytes = 560

    Material::Ptr m_DebuggingAABBMat;

    struct Triangle
    {
        vec3 pt[3];
        bool culled;
    };
};
