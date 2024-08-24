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

    void AddSceneNode(SceneNode::Ptr sceneNode);
    void ExecuteCommandBuffer();
    void RenderCommand(RenderCommand::Ptr command);
    void RenderMesh(Mesh::Ptr mesh);
    
    void CalculateSceneAABB();

private:

    void DrawSceneNode(SceneNode::Ptr sceneNode);

    void AddRenderLightCommand(Light::Ptr light);
    void BuildSkyboxRenderCommands();

    void LoadEnvironment(const std::string &cubemapPath);
    void GenerateBRDFLUT();
    void GenerateCubemaps();
    void RenderToCubemap(TextureCube::Ptr cubemap, unsigned int mipLevel = 0);

    void BuildRenderCommands(SceneNode::Ptr sceneNode);
    
    void ComputeShadowProjectionFitViewFrustum(const glm::mat4 &cameraProjection, const glm::mat4 &cameraView, const glm::mat4 &lightView,vec3 &lightCameraOrthographicMin, vec3 &lightCameraOrthographicMax);

    void SetGLCull(bool enable);
    void SetGLBlend(bool enable);

    void Blit(Texture2D::Ptr source, RenderTarget::Ptr destination, Material::Ptr blitMat);

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
    
    GLuint m_BlitVAO;
    Material::Ptr m_BlitMat;

    // Environments
    GLuint m_FrameBufferID;
    GLuint m_CubemapDepthRenderBufferID;
    SceneNode::Ptr m_Cube;
    TextureCube::Ptr m_EnvironmentCubemap;
    TextureCube::Ptr m_IrradianceCubemap;
    TextureCube::Ptr m_PrefilteredCubemap;
    RenderTarget::Ptr m_BRDFLUTRT;

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
