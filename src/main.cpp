#include <string>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "cameras/ArcballCamera.h"

#include "lights/DirectionalLight.h"

#include "loader/AssetsLoader.h"

#include "renderer/RenderNode.h"

#include "SceneRenderGraph.h"

//#include <iomanip>

const int WIDTH = 1280;
const int HEIGHT = 720;

float m_LastX = WIDTH / 2.0f;
float m_LastY = HEIGHT / 2.0f;
bool m_LeftMouseButtonPressed = false;
bool m_RightMouseButtonPressed = false;

GLFWwindow* m_Window;
SceneRenderGraph::Ptr m_SceneRenderGraph;

// Window callbacks
void errorCallback(int error, const char* description);
void resizeCallback(GLFWwindow* window, int width, int height);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double x, double y);
void processWindowInput(GLFWwindow* window);
void checkOpenGLError();

int main()
{
//    double p = 3.1415926535897932;
//    std::cout << std::fixed << std::setprecision(16);
//    std::cout << 1.0 / p << std::endl;

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

    // cannot use any features that have been marked as deprecated
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#ifdef __APPLE__
    // Retina support on macOS
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
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

    // SceneRenderGraph
    m_SceneRenderGraph = SceneRenderGraph::New();
    m_SceneRenderGraph->init();

//     RenderNode::Ptr renderNode = AssetsLoader::load_glTF("models/glTF/DamagedHelmet/glTF/DamagedHelmet.gltf");
//    RenderNode::Ptr renderNode = AssetsLoader::load_glTF("models/glTF/buster_drone/busterDrone.gltf");
//     RenderNode::Ptr renderNode = AssetsLoader::load_glTF("models/glTF/FlightHelmet/glTF/FlightHelmet.gltf");
    // RenderNode::Ptr renderNode = AssetsLoader::load_glTF("models/glTF/DragonAttenuation/glTF/DragonAttenuation.gltf");
//     RenderNode::Ptr renderNode = AssetsLoader::load_glTF("models/glTF/cube/cube2.gltf");
//     m_SceneRenderGraph->pushRenderNode(renderNode);

    RenderNode::Ptr marry = AssetsLoader::loadObj("models/obj/mary/Marry.obj");
    marry->translate(glm::vec3(0.0f, -1.5f, 0.0f));
    m_SceneRenderGraph->pushRenderNode(marry);

    RenderNode::Ptr floor = AssetsLoader::loadObj("models/obj/floor/floor.obj");
    floor->translate(glm::vec3(0.0f, -1.5f, 0.0f));
    m_SceneRenderGraph->pushRenderNode(floor);

    ArcballCamera::Ptr camera = ArcballCamera::perspectiveCamera(glm::radians(60.0f), WIDTH, HEIGHT, 0.001f, 1000.0f);
    m_SceneRenderGraph->setCamera(camera);

    // Get the real size in pixels
    int width, height;
    glfwGetFramebufferSize(m_Window, &width, &height);
    glViewport(0, 0, width, height);
    m_SceneRenderGraph->setRenderSize(width, height);

    DirectionalLight::Ptr light = DirectionalLight::New(glm::vec3(2.0f, 1.5f, 1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    m_SceneRenderGraph->addLight(light);

    while (!glfwWindowShouldClose(m_Window))
    {
        processWindowInput(m_Window);

        m_SceneRenderGraph->executeCommandBuffer();

        checkOpenGLError();

        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void checkOpenGLError()
{
    GLenum err = glGetError();
    while (err != GL_NO_ERROR) {
        const char* error;
        switch (err) {
        case GL_INVALID_OPERATION:      error = "INVALID_OPERATION"; break;
        case GL_INVALID_ENUM:           error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:          error = "INVALID_VALUE"; break;
        case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        default:                        error = "UNKNOWN_ERROR"; break;
        }
        fprintf(stderr, "OpenGL error: %s\n", error);
        err = glGetError();
    }
}

void errorCallback(int error, const char* description)
{
    std::cerr << "glfw error: " << description << std::endl;
}

void resizeCallback(GLFWwindow* window, int width, int height)
{
    if (width > 0.0f && height > 0.0f)
    {
        m_SceneRenderGraph->setRenderSize(width, height);
    }

    glViewport(0, 0, width, height);
}

void processWindowInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
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

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            m_RightMouseButtonPressed = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_RightMouseButtonPressed = false;
        }
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
        m_SceneRenderGraph->getActiveCamera()->arcballing(dx, dy);
    }

    if (m_RightMouseButtonPressed)
    {
        m_SceneRenderGraph->getActiveCamera()->panning(dx, -dy);
    }
}
