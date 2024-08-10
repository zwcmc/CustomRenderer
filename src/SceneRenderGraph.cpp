#include "SceneRenderGraph.h"

#include <stack>

#include "base/TextureCube.h"
#include "loader/AssetsLoader.h"

SceneRenderGraph::SceneRenderGraph()
    : m_GlobalUniformBufferID(0), m_CullFace(true), m_Blend(false), m_RenderSize(glm::u32vec2(1))
{ }

SceneRenderGraph::~SceneRenderGraph()
{
    m_RenderNodes.clear();
    m_Lights.clear();
}

void SceneRenderGraph::init()
{
    m_CommandBuffer = CommandBuffer::New();

    // No seams at cubemap edges
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // Set clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Depth test
    glEnable(GL_DEPTH_TEST);

    // Default cull face state
    m_CullFace = true;
    glEnable(GL_CULL_FACE);

    // Default blend state
    m_Blend = false;
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Global uniform buffer object
    glGenBuffers(1, &m_GlobalUniformBufferID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_GlobalUniformBufferID);
    glBufferData(GL_UNIFORM_BUFFER, 176, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_GlobalUniformBufferID); // Set global uniform to binding point 0
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Mesh for rendering lights
    m_LightMesh = Sphere::New(2, 2, 0.02f);

    m_BlitMat = Material::New("Blit", "glsl_shaders/Blit.vs", "glsl_shaders/Blit.fs");
    m_RenderTarget = RenderTarget::New(glm::u32vec2(1), GL_HALF_FLOAT);

    // Load skybox
    // loadEnvironment("textures/environments/cubemap_yokohama_rgba.ktx");
    loadEnvironment("textures/environments/hdr/newport_loft.hdr");
}

void SceneRenderGraph::setRenderSize(int width, int height)
{
    m_RenderSize.x = width;
    m_RenderSize.y = height;

    m_Camera->setScreenSize(width, height);

    m_RenderTarget->resize(glm::u32vec2(width, height)); 
}

void SceneRenderGraph::setCamera(ArcballCamera::Ptr camera)
{
    m_Camera = camera;
}

void SceneRenderGraph::addLight(BaseLight::Ptr light)
{
    m_Lights.push_back(light);

    // Add a new render command for render light
    addRenderLightCommand(light);
}

void SceneRenderGraph::pushRenderNode(RenderNode::Ptr renderNode)
{
    m_RenderNodes.push_back(renderNode);

    // Build render commands
    buildRenderCommands(renderNode);
}

void SceneRenderGraph::addRenderLightCommand(BaseLight::Ptr light)
{
    // TODO: All lights can share a single material
    Material::Ptr lightMat = Material::New("Emissive", "glsl_shaders/Emissive.vs", "glsl_shaders/Emissive.fs");
    lightMat->addOrSetVector("uEmissiveColor", light->getLightColor());

    // Only need to modify the translation column
    glm::mat4 transform = glm::mat4(1.0f);
    glm::vec3 lightPos = light->getLightPosition();
    transform[3][0] = lightPos.x;
    transform[3][1] = lightPos.y;
    transform[3][2] = lightPos.z;
    m_CommandBuffer->pushCommand(m_LightMesh, lightMat, transform);
}

void SceneRenderGraph::renderToCubemap(Texture2D::Ptr envMap, TextureCube::Ptr cubemap)
{
    glm::u32vec2 size = cubemap->getSize();

    // render texture to cubemap
    GLuint frameBufferID;
    glGenFramebuffers(1, &frameBufferID);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

    GLuint cubemapDepthRenderBufferID;
    glGenRenderbuffers(1, &cubemapDepthRenderBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, cubemapDepthRenderBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size.x, size.y);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, cubemapDepthRenderBufferID);

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "FrameBuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), (float)size.x / size.y, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // Configure the viewport to the capture dimensions
    glViewport(0, 0, size.x, size.y);
    
    // Bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

    Material::Ptr capMat = Material::New("HDR_to_Cubemap", "glsl_shaders/Skybox.vs", "glsl_shaders/HDRToCubemap.fs");
    capMat->addOrSetTexture("uEnvMap", envMap);
    m_Skybox->setOverrideMaterial(capMat);

    // Vertex shader output gl_Postion = clipPos.xyww, depth is maximum 1.0, so use less&equal depth func
    glDepthFunc(GL_LEQUAL);

    // Importent: Render the spherical position to the cube position inside the inner cube box; cull face must be disabled
    glDisable(GL_CULL_FACE);

    // Bind global uniforms
    glBindBuffer(GL_UNIFORM_BUFFER, m_GlobalUniformBufferID);

    glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &(captureProjection[0].x));

    for (unsigned int i = 0; i < 6; ++i)
    {
        // Set global uniforms
        glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &(captureViews[i][0].x));

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap->getTextureID(), 0);
        // Check framebuffer status
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "FrameBuffer is not complete!" << std::endl;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawNode(m_Skybox);
    }

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Unbind global uniforms
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Set back to default
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    glViewport(0, 0, m_RenderSize.x, m_RenderSize.y);
}

void SceneRenderGraph::drawNode(RenderNode::Ptr node)
{
    Material::Ptr overrideMat = node->OverrideMat;
    for (size_t i = 0; i < node->MeshRenders.size(); ++i)
    {
        MeshRender::Ptr mr = node->MeshRenders[i];
        if (overrideMat)
        {
            overrideMat->use();
        }
        else
        {
            mr->getMaterial()->use();
        }
        renderMesh(mr->getMesh());
    }

    for (size_t i = 0; i < node->Children.size(); ++i)
    {
        drawNode(node->Children[i]);
    }
}

void SceneRenderGraph::loadEnvironment(const std::string &cubemapPath)
{
    std::string fileExt;
    size_t extPos = cubemapPath.rfind('.', cubemapPath.length());
    if (extPos != std::string::npos)
    {
        fileExt = cubemapPath.substr(extPos + 1, cubemapPath.length());
    }
    else
    {
        std::cerr << "Cubemap file path is wrong, path is: " << cubemapPath << std::endl;
        return;
    }

    m_Skybox = AssetsLoader::load_glTF("models/Box/glTF-Embedded/Box.gltf");
    TextureCube::Ptr cubemap = TextureCube::New("uCubemap");
    if (fileExt == "ktx")
    {
        cubemap = AssetsLoader::loadCubemapKTX("uCubemap", cubemapPath);
    }
    else if (fileExt == "hdr")
    {
        Texture2D::Ptr environmentMap = AssetsLoader::loadHDRTexture("uEnvMap", cubemapPath);
        cubemap->defaultInit(512, 512, GL_RGB32F, GL_RGB, GL_FLOAT);
        // Equirectangular map to a cubemap
        renderToCubemap(environmentMap, cubemap);
    }
    else
    {
        std::cerr << "Unsupport cubemap file, path is: " << cubemapPath << std::endl;
        return;
    }

    Material::Ptr skyboxMat = Material::New("Skybox", "glsl_shaders/Skybox.vs", "glsl_shaders/Skybox.fs", true);
    skyboxMat->addOrSetTextureCube(cubemap);

    m_Skybox->setOverrideMaterial(skyboxMat);
    buildRenderCommands(m_Skybox);
}

void SceneRenderGraph::buildRenderCommands(RenderNode::Ptr renderNode)
{
    glm::mat4 model = renderNode->getModelMatrix();
    Material::Ptr overrideMat = renderNode->OverrideMat;
    for (size_t i = 0; i < renderNode->MeshRenders.size(); ++i)
    {
        m_CommandBuffer->pushCommand(renderNode->MeshRenders[i]->getMesh(), overrideMat ? overrideMat : renderNode->MeshRenders[i]->getMaterial(), model);
    }

    for (size_t i = 0; i < renderNode->Children.size(); ++i)
    {
        buildRenderCommands(renderNode->Children[i]);
    }
}

void SceneRenderGraph::executeCommandBuffer()
{
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

    // Bind framebuffer
    m_RenderTarget->bind();

    // Opaque
    std::vector<RenderCommand::Ptr> opaqueCommands = m_CommandBuffer->getOpaqueCommands();
    for (size_t i = 0; i < opaqueCommands.size(); ++i)
    {
       RenderCommand::Ptr command = opaqueCommands[i];
       renderCommand(command);
    }

    // Skybox start ----------------
    // Skybox's depth always is 1.0, is equal to the max depth buffer, rendering skybox after opauqe objects and setting depth func to less&equal will
    // ensure that the skybox is only renderered in pixels that are not covered by the opaque objects.
    // Pixels covered by opaque objects have a depth less than 1.0. Therefore, the depth test will never pass when rendering the skybox.
    glDepthFunc(GL_LEQUAL);
    // Depth write off
    glDepthMask(GL_FALSE);

    std::vector<RenderCommand::Ptr> skyboxCommands = m_CommandBuffer->getSkyboxCommands();
    for (size_t i = 0; i < skyboxCommands.size(); ++i)
    {
        RenderCommand::Ptr command = skyboxCommands[i];
        renderCommand(command);
    }

    // Depth write on
    glDepthMask(GL_TRUE);
    // Set back to less
    glDepthFunc(GL_LESS);
    // Skybox end ----------------

    // Transparent
    std::vector<RenderCommand::Ptr> transparentCommands = m_CommandBuffer->getTransparentCommands();
    for (size_t i = 0; i < transparentCommands.size(); ++i)
    {
       RenderCommand::Ptr command = transparentCommands[i];
       renderCommand(command);
    }

    blitToScreen(m_RenderTarget->getColorTexture(0));
}

void SceneRenderGraph::renderCommand(RenderCommand::Ptr command)
{
    Mesh::Ptr mesh = command->Mesh;
    Material::Ptr mat = command->Material;

    setGLCull(mat->getDoubleSided());
    setGLBlend(mat->getAlphaMode() == Material::AlphaMode::BLEND);

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

void SceneRenderGraph::setGLCull(bool enable)
{
    if (m_CullFace != enable)
    {
        m_CullFace = enable;
        if (enable)
        {
            glEnable(GL_CULL_FACE);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
    }
}

void SceneRenderGraph::setGLBlend(bool enable)
{
    if (m_Blend != enable)
    {
        m_Blend = enable;
        if (enable)
        {
            glEnable(GL_BLEND);
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }
}

void SceneRenderGraph::blitToScreen(Texture2D::Ptr source)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_RenderSize.x, m_RenderSize.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_BlitMat->addOrSetTexture(source);
    m_BlitMat->use();

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}