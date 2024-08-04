#include <string>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "cameras/ArcballCamera.h"

#include "lights/DirectionalLight.h"

#include "loader/AssetsLoader.h"

#include "renderer/glTFRenderer.h"
#include "renderer/ShapeRenderer.h"
#include "SceneGraph.h"

const int WIDTH = 1280;
const int HEIGHT = 720;

float m_LastX = WIDTH / 2.0f;
float m_LastY = HEIGHT / 2.0f;
bool m_LeftMouseButtonPressed = false;
bool m_MiddleMouseButtonPressed = false;
bool m_RightMouseButtonPressed = false;

GLFWwindow* m_Window;
SceneGraph::Ptr m_SceneRenderGraph;

// Window callbacks
void errorCallback(int error, const char* description);
void resizeCallback(GLFWwindow* window, int width, int height);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double x, double y);
void processWindowInput(GLFWwindow* window);

int main()
{
    glfwSetErrorCallback(errorCallback);

    // Init glfw
    if (!glfwInit())
    {
        std::cerr << "Failed to init glfw" << std::endl;
        return -1;
    }

    // Setup glfw
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    m_Window = glfwCreateWindow(WIDTH, HEIGHT, "CustomRenderer", nullptr, nullptr);
    if (!m_Window)
    {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(m_Window);

    // Set callbacks
    glfwSetFramebufferSizeCallback(m_Window, resizeCallback);
    glfwSetScrollCallback(m_Window, scrollCallback);
    glfwSetMouseButtonCallback(m_Window, mouseButtonCallback);
    glfwSetCursorPosCallback(m_Window, cursorPositionCallback);

    // Load all opengl function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to init glad" << std::endl;
        return -1;
    }

    // Cull face
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Depth test
    glEnable(GL_DEPTH_TEST);

    // SceneGraph
    m_SceneRenderGraph = SceneGraph::New();

    // glTF models
    glTFRenderer::Ptr glTFModelRenderer = AssetsLoader::loadglTFFile("models/DamagedHelmet/glTF/DamagedHelmet.gltf");
    // glTFRenderer::Ptr glTFModelRenderer = AssetsLoader::loadglTFFile("models/buster_drone/busterDrone.gltf");
    m_SceneRenderGraph->addModelRenderer(glTFModelRenderer);

    float aspectRatio = static_cast<float>(WIDTH) / HEIGHT;
    ArcballCamera::Ptr camera = ArcballCamera::perspectiveCamera(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);
    camera->setScreenSize(WIDTH, HEIGHT);
    m_SceneRenderGraph->setCamera(camera);

    DirectionalLight::Ptr light = DirectionalLight::New(glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
    m_SceneRenderGraph->addLight(light);

    while (!glfwWindowShouldClose(m_Window))
    {
        processWindowInput(m_Window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_SceneRenderGraph->render();

        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void errorCallback(int error, const char* description)
{
    std::cerr << "glfw error: " << description << std::endl;
}

void resizeCallback(GLFWwindow* window, int width, int height)
{
    if (width > 0.0f && height > 0.0f)
    {
        float aspect = static_cast<float>(width) / height;
        m_SceneRenderGraph->getActiveCamera()->setAspectRatio(aspect);
    }

    glViewport(0, 0, width, height);
}

void processWindowInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    m_SceneRenderGraph->getActiveCamera()->zooming(static_cast<float>(yoffset));
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
            m_LeftMouseButtonPressed = true;
        else if (action == GLFW_RELEASE)
            m_LeftMouseButtonPressed = false;
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        if (action == GLFW_PRESS)
            m_MiddleMouseButtonPressed = true;
        else if (action == GLFW_RELEASE)
            m_MiddleMouseButtonPressed = false;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
            m_RightMouseButtonPressed = true;
        else if (action == GLFW_RELEASE)
            m_RightMouseButtonPressed = false;
    }
}

void cursorPositionCallback(GLFWwindow* window, double x, double y)
{
    float xpos = static_cast<float>(x);
    float ypos = static_cast<float>(y);

    float dx = m_LastX - xpos;
    float dy = m_LastY - ypos;

    m_LastX = xpos;
    m_LastY = ypos;

    if (m_LeftMouseButtonPressed)
    {
        m_SceneRenderGraph->rotateModelRenderers(glm::vec3(dy, -dx, 0.0f));
    }
    
    if (m_MiddleMouseButtonPressed)
    {
        m_SceneRenderGraph->getActiveCamera()->panning(dx, -dy);
    }

    if (m_RightMouseButtonPressed)
    {
        m_SceneRenderGraph->getActiveCamera()->arcballing(dx, dy);
    }
}