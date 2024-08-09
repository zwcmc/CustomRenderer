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

    std::vector<RenderCommand::Ptr>& getOpaqueCommands() { return m_OpaqueCommands; }
    std::vector<RenderCommand::Ptr>& getSkyboxCommands() { return m_SkyboxCommands; }
    std::vector<RenderCommand::Ptr>& getTransparentCommands() { return m_TransparentCommands; }

private:
    std::vector<RenderCommand::Ptr> m_OpaqueCommands;
    std::vector<RenderCommand::Ptr> m_SkyboxCommands;
    std::vector<RenderCommand::Ptr> m_TransparentCommands;
};