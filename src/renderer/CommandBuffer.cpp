#include "CommandBuffer.h"

CommandBuffer::~CommandBuffer()
{
    clear();
}

void CommandBuffer::clear()
{
    m_OpaqueRenderCommands.clear();
    m_TransparentRenderCommands.clear();
}

void CommandBuffer::pushCommand(Mesh::Ptr mesh, Material::Ptr mat, glm::mat4 transform)
{
    RenderCommand::Ptr cmd = RenderCommand::New();
    cmd->Mesh = mesh;
    cmd->Material = mat;
    cmd->Transform = transform;

    if (mat->getAlphaMode() == Material::AlphaMode::BLEND)
    {
        m_TransparentRenderCommands.push_back(cmd);
    }
    else
    {
        m_OpaqueRenderCommands.push_back(cmd);
    }
}