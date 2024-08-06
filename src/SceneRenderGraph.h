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

class SceneRenderGraph
{
    SHARED_PTR(SceneRenderGraph)
public:
    SceneRenderGraph();
    ~SceneRenderGraph();

    void setCamera(ArcballCamera::Ptr camera);
    ArcballCamera::Ptr getActiveCamera() { return m_Camera; }

    void addLight(BaseLight::Ptr light);

    void init();

    void pushRenderNode(RenderNode::Ptr renderNode);
    void executeCommandBuffer();
    void renderCommand(RenderCommand::Ptr command);
    void renderMesh(Mesh::Ptr mesh);

private:

    void renderLight(BaseLight::Ptr light);

    // Should match GlobalUniforms in Uniforms.glsl
    // struct GlobalUniforms
    // {
    //     glm::mat4 view;        // 64 bytes;  byte offset = 0;
    //     glm::mat4 projection;  // 64 bytes;  byte offset = 64;
    //     vec3 lightDirection0;  // 16 bytes;  byte offset = 128;
    //     vec3 lightColor0;      // 16 bytes;  byte offset = 144;
    //     vec3 cameraPos;        // 16 bytes;  byte offset = 160;
    // };                         // total bytes = 176

    CommandBuffer::Ptr m_CommandBuffer;
    ArcballCamera::Ptr m_Camera;
    std::vector<BaseLight::Ptr> m_Lights;

    GLuint m_GlobalUniformBufferID;

    Mesh::Ptr m_LightSphere;
    Material::Ptr m_LightMaterial;
};