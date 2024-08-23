#pragma once

#include <iostream>
#include <vector>

#include <glad/glad.h>

#include "ptr.h"

#include "cameras/Camera.h"
#include "lights/Light.h"

#include "renderer/RenderCommand.h"
#include "renderer/CommandBuffer.h"
#include "renderer/SceneNode.h"

#include "meshes/Sphere.h"

#include "renderer/RenderTarget.h"

class SceneRenderGraph
{
    SHARED_PTR(SceneRenderGraph)
public:
    SceneRenderGraph();
    ~SceneRenderGraph();

    void setRenderSize(const int &width, const int &height);
    void setCamera(Camera::Ptr camera);
    Camera::Ptr getActiveCamera() { return m_Camera; }

    void addLight(Light::Ptr light);

    void init();

    void pushRenderNode(SceneNode::Ptr sceneNode);
    void executeCommandBuffer();
    void renderCommand(RenderCommand::Ptr command);
    void renderMesh(Mesh::Ptr mesh);

private:

    void drawRenderNode(SceneNode::Ptr sceneNode);

    void addRenderLightCommand(Light::Ptr light);
    void buildSkyboxRenderCommands();

    void loadEnvironment(const std::string &cubemapPath);
    void generateBRDFLUT();
    void generateCubemaps();
    void renderToCubemap(TextureCube::Ptr cubemap, unsigned int mipLevel = 0);

    void buildRenderCommands(SceneNode::Ptr sceneNode);

    void setGLCull(bool enable);
    void setGLBlend(bool enable);

    void blit(Texture2D::Ptr source, RenderTarget::Ptr destination, Material::Ptr blitMat);

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
