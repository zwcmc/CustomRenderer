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

    // // std140 layout
    // struct UniformData
    // {
    //     glm::mat4 view;        // 64 bytes;  byte offset = 0;
    //     glm::mat4 projection;  // 64 bytes;  byte offset = 64;
    // };

    CommandBuffer::Ptr m_CommandBuffer;

    ArcballCamera::Ptr m_Camera;

    std::vector<BaseLight::Ptr> m_Lights;

    GLuint m_GlobalUniformBufferID;
};