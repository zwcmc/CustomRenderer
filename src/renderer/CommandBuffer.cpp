#include "CommandBuffer.h"

CommandBuffer::~CommandBuffer()
{
    Clear();
}

void CommandBuffer::Clear()
{
    m_OpaqueCommands.clear();
    m_SkyboxCommands.clear();
    m_TransparentCommands.clear();
    m_DebuggingCommands.clear();
}

void CommandBuffer::PushCommand(Mesh::Ptr mesh, Material::Ptr mat, glm::mat4 transform)
{
    RenderCommand::Ptr cmd = RenderCommand::New();
    cmd->Mesh = mesh;
    cmd->Material = mat;
    cmd->Transform = transform;

    if (mat->IsUsedForSkybox())
        m_SkyboxCommands.push_back(cmd);
    else
    {
        if (mat->GetAlphaMode() == Material::AlphaMode::BLEND)
            m_TransparentCommands.push_back(cmd);
        else
            m_OpaqueCommands.push_back(cmd);
    }
}

void CommandBuffer::PushDebuggingCommand(Mesh::Ptr mesh, Material::Ptr mat, glm::mat4 transform)
{
    RenderCommand::Ptr cmd = RenderCommand::New();
    cmd->Mesh = mesh;
    cmd->Material = mat;
    cmd->Transform = transform;

    m_DebuggingCommands.push_back(cmd);
}

std::vector<RenderCommand::Ptr> CommandBuffer::GetShadowCasterCommands()
{
    std::vector<RenderCommand::Ptr> commands;
    for (size_t i = 0; i < m_OpaqueCommands.size(); ++i)
    {
        if (m_OpaqueCommands[i]->Material->GetMaterialCastShadows())
        {
            commands.push_back(m_OpaqueCommands[i]);
        }
    }
    for (size_t i = 0; i < m_TransparentCommands.size(); ++i)
    {
        if (m_TransparentCommands[i]->Material->GetMaterialCastShadows())
        {
            commands.push_back(m_TransparentCommands[i]);
        }
    }
    return commands;
}
