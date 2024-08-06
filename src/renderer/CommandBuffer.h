#pragma once

#include <vector>

#include "ptr.h"
#include "renderer/RenderCommand.h"

class CommandBuffer
{
    SHARED_PTR(CommandBuffer)
public:
    CommandBuffer() = default;
    ~CommandBuffer();

    void pushCommand(Mesh::Ptr mesh, Material::Ptr mat, glm::mat4 transform = glm::mat4(1.0f));
    void clear();

    std::vector<RenderCommand::Ptr> getOpaqueRenderCommands() { return m_OpaqueRenderCommands; }
    std::vector<RenderCommand::Ptr> getTransparentRenderCommands() { return m_TransparentRenderCommands; }

private:
    std::vector<RenderCommand::Ptr> m_OpaqueRenderCommands;
    std::vector<RenderCommand::Ptr> m_TransparentRenderCommands;
};