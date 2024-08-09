#pragma once

#include <iostream>
#include <vector>

#include <glad/glad.h>

#include "ptr.h"

#include "cameras/ArcballCamera.h"
#include "lights/BaseLight.h"

#include "renderer/RenderCommand.h"
#include "renderer/CommandBuffer.h"
#include "renderer/RenderNode.h"

#include "meshes/Sphere.h"

#include "renderer/RenderTarget.h"

class SceneRenderGraph
{
    SHARED_PTR(SceneRenderGraph)
public:
    SceneRenderGraph();
    ~SceneRenderGraph();

    void setRenderSize(int width, int height);
    void setCamera(ArcballCamera::Ptr camera);
    ArcballCamera::Ptr getActiveCamera() { return m_Camera; }

    void addLight(BaseLight::Ptr light);

    void init();

    void pushRenderNode(RenderNode::Ptr renderNode);
    void executeCommandBuffer();
    void renderCommand(RenderCommand::Ptr command);
    void renderMesh(Mesh::Ptr mesh);

private:

    void addRenderLightCommand(BaseLight::Ptr light);
    void loadEnvironment(const std::string &cubemapPath);
    void buildRenderCommands(RenderNode::Ptr renderNode);

    void setGLCull(bool enable);
    void setGLBlend(bool enable);

    void blitToScreen(Texture2D::Ptr texture);

    bool m_CullFace;
    bool m_Blend;

    // Should match GlobalUniforms in Uniforms.glsl
    // struct GlobalUniforms
    // {
    //     glm::mat4 view;        // 64 bytes;  byte offset = 0;
    //     glm::mat4 projection;  // 64 bytes;  byte offset = 64;
    //     vec3 lightDirection0;  // 16 bytes;  byte offset = 128;
    //     vec3 lightColor0;      // 16 bytes;  byte offset = 144;
    //     vec3 cameraPos;        // 16 bytes;  byte offset = 160;
    // };                         // total bytes = 176

    glm::u32vec2 m_RenderSize;

    std::vector<RenderNode::Ptr> m_RenderNodes;

    CommandBuffer::Ptr m_CommandBuffer;
    ArcballCamera::Ptr m_Camera;
    std::vector<BaseLight::Ptr> m_Lights;

    GLuint m_GlobalUniformBufferID;

    Sphere::Ptr m_LightMesh;

    RenderNode::Ptr m_Skybox;

    RenderTarget::Ptr m_RenderTarget;
    Material::Ptr m_BlitMat;
};