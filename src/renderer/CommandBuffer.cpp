#include "CommandBuffer.h"

CommandBuffer::~CommandBuffer()
{
    clear();
}

void CommandBuffer::clear()
{
    m_OpaqueCommands.clear();
    m_SkyboxCommands.clear();
    m_TransparentCommands.clear();
}

void CommandBuffer::pushCommand(Mesh::Ptr mesh, Material::Ptr mat, glm::mat4 transform)
{
    RenderCommand::Ptr cmd = RenderCommand::New();
    cmd->Mesh = mesh;
    cmd->Material = mat;
    cmd->Transform = transform;

    if (mat->isUsedForSkybox())
    {
        m_SkyboxCommands.push_back(cmd);
    }
    else
    {
        if (mat->getAlphaMode() == Material::AlphaMode::BLEND)
        {
            m_TransparentCommands.push_back(cmd);
        }
        else
        {
            m_OpaqueCommands.push_back(cmd);
        }
    }
}