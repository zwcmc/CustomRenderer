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

    void PushCommand(Mesh::Ptr mesh, Material::Ptr mat, glm::mat4 transform = glm::mat4(1.0f));
    void PushDebuggingCommand(Mesh::Ptr mesh, Material::Ptr mat, glm::mat4 transform = glm::mat4(1.0f));
    void Clear();

    std::vector<RenderCommand::Ptr>& GetOpaqueCommands();
    std::vector<RenderCommand::Ptr>& GetSkyboxCommands();
    std::vector<RenderCommand::Ptr>& GetTransparentCommands();
    std::vector<RenderCommand::Ptr> GetShadowCasterCommands();
    std::vector<RenderCommand::Ptr>& GetDebuggingCommands();

private:
    std::vector<RenderCommand::Ptr> m_OpaqueCommands;
    std::vector<RenderCommand::Ptr> m_SkyboxCommands;
    std::vector<RenderCommand::Ptr> m_TransparentCommands;

    std::vector<RenderCommand::Ptr> m_DebuggingCommands;
};
