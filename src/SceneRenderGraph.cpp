#include "SceneRenderGraph.h"

#include <stack>

SceneRenderGraph::SceneRenderGraph()
    : m_GlobalUniformBufferID(0)
{ }

SceneRenderGraph::~SceneRenderGraph()
{
    m_Lights.clear();
}

void SceneRenderGraph::init()
{
    m_CommandBuffer = CommandBuffer::New();

    // Depth test
    glEnable(GL_DEPTH_TEST);

    // Default cull face state
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glDisable(GL_BLEND);

    // Global uniform buffer object
    glGenBuffers(1, &m_GlobalUniformBufferID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_GlobalUniformBufferID);
    glBufferData(GL_UNIFORM_BUFFER, 176, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_GlobalUniformBufferID); // Set global uniform to binding point 0
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void SceneRenderGraph::setCamera(ArcballCamera::Ptr camera)
{
    m_Camera = camera;
}

void SceneRenderGraph::addLight(BaseLight::Ptr light)
{
    m_Lights.push_back(light);
}

void SceneRenderGraph::pushRenderNode(RenderNode::Ptr renderNode)
{
    std::stack<RenderNode::Ptr> nodeStack;
    nodeStack.push(renderNode);
    for (size_t i = 0; i < renderNode->Children.size(); ++i)
    {
        nodeStack.push(renderNode->Children[i]);
    }
    while (!nodeStack.empty())
    {
        RenderNode::Ptr node = nodeStack.top();
        nodeStack.pop();

        glm::mat4 model = node->getModelMatrix();
        for (size_t i = 0; i < node->MeshRenders.size(); ++i)
        {
            m_CommandBuffer->pushCommand(node->MeshRenders[i]->getMesh(), node->MeshRenders[i]->getMaterial(), model);
        }

        for (size_t i = 0; i < node->Children.size(); ++i)
        {
            nodeStack.push(node->Children[i]);
        }
    }
}

void SceneRenderGraph::executeCommandBuffer()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 v = m_Camera->getViewMatrix();
    glm::mat4 p = m_Camera->getProjectionMatrix();
    
    BaseLight::Ptr light0 = m_Lights[0];
    glm::vec3 lightDir0 = light0->getLightPosition();
    glm::vec3 lightColor0 = light0->getLightColor();
    glm::vec3 cameraPos = m_Camera->getPosition();

    // Set global uniforms
    glBindBuffer(GL_UNIFORM_BUFFER, m_GlobalUniformBufferID);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &(v[0].x));
    glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &(p[0].x));
    glBufferSubData(GL_UNIFORM_BUFFER, 128, 16, &(lightDir0.x));
    glBufferSubData(GL_UNIFORM_BUFFER, 144, 16, &(lightColor0.x));
    glBufferSubData(GL_UNIFORM_BUFFER, 160, 16, &(cameraPos.x));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Opaque
    std::vector<RenderCommand::Ptr> opaqueCommands = m_CommandBuffer->getOpaqueRenderCommands();
    for (size_t i = 0; i < opaqueCommands.size(); ++i)
    {
        RenderCommand::Ptr command = opaqueCommands[i];
        renderCommand(command);
    }

    // Transparent
    std::vector<RenderCommand::Ptr> transparentCommands = m_CommandBuffer->getTransparentRenderCommands();
    for (size_t i = 0; i < transparentCommands.size(); ++i)
    {
        RenderCommand::Ptr command = transparentCommands[i];
        renderCommand(command);
    }
}

void SceneRenderGraph::renderCommand(RenderCommand::Ptr command)
{
    Mesh::Ptr mesh = command->Mesh;
    Material::Ptr mat = command->Material;

    if (mat->getDoubleSided())
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    if (mat->getAlphaMode() == Material::AlphaMode::BLEND)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        glDisable(GL_BLEND);
    }

    mat->use();
    mat->setMatrix("uModelMatrix", command->Transform);
    mat->setMatrix("uModelMatrixInverse", glm::mat3x3(glm::inverse(command->Transform)));

    renderMesh(mesh);
}

void SceneRenderGraph::renderMesh(Mesh::Ptr mesh)
{
    glBindVertexArray(mesh->getVertexArrayID());

    if (mesh->getIndicesCount() > 0)
    {
        glDrawElements(GL_TRIANGLES, mesh->getIndicesCount(), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, mesh->getVerticesCount());
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}