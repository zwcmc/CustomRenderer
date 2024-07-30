#include <string>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderer/Renderer.h"
#include "base/ShaderProgram.h"

#include "model/Cube.h"

const int WIDTH = 1280;
const int HEIGHT = 720;

float m_LastX = WIDTH / 2.0f;
float m_LastY = HEIGHT / 2.0f;
bool m_LeftMouseButtonPressed = false;
bool m_MiddleMouseButtonPressed = false;

GLFWwindow* m_Window;
Renderer* m_Renderer;

// Window callback
void errorCallback(int error, const char* description);
void resizeCallback(GLFWwindow* window, int width, int height);
void processWindowInput(GLFWwindow* window);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void processInput(GLFWwindow* window);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double x, double y);

int main()
{
    glfwSetErrorCallback(errorCallback);

    // Init glfw
    if (!glfwInit())
    {
        std::cout << "Failed to init glfw" << std::endl;
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
    m_Window = glfwCreateWindow(WIDTH, HEIGHT, "CustomRenderer", NULL, NULL);
    if (!m_Window)
    {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(m_Window);

    glfwSetFramebufferSizeCallback(m_Window, resizeCallback);

    // Camera update
    glfwSetScrollCallback(m_Window, scrollCallback);
    glfwSetMouseButtonCallback(m_Window, mouseButtonCallback);
    glfwSetCursorPosCallback(m_Window, cursorPositionCallback);

    // Load all opengl function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init glad" << std::endl;
        return -1;
    }

    Shader vertexShader = Shader::fromFile("glsl_shaders/Default.vert", Shader::ShaderType::VERTEX);
    Shader fragmentShader = Shader::fromFile("glsl_shaders/Default.frag", Shader::ShaderType::FRAGMENT);
    ShaderProgram* program = new ShaderProgram("Default", { vertexShader, fragmentShader });

    Cube *cube = new Cube();

    // Renderer
    m_Renderer = new Renderer();

    float aspectRatio = static_cast<float>(WIDTH) / HEIGHT;
    Camera* camera = Camera::perspectiveCamera(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);
    m_Renderer->setCamera(camera);

    while (!glfwWindowShouldClose(m_Window))
    {
        processWindowInput(m_Window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw our first triangle
        program->useProgram();

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 p = camera->getProjectionMatrix();
        glm::mat4 v = camera->getViewMatrix();
        program->setUniform("model", model);
        program->setUniform("view", v);
        program->setUniform("projection", p);

        cube->draw();

        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void errorCallback(int error, const char* description)
{
    std::cout << "glfw error: " << description << std::endl;
}

void resizeCallback(GLFWwindow* window, int width, int height)
{
    if (width > 0.0f && height > 0.0f)
        m_Renderer->updateCamera(Renderer::CameraUpdateType::ASPECT_RATIO, glm::vec3(static_cast<float>(width), static_cast<float>(height), 0.0f));

    glViewport(0, 0, width, height);
}

void processWindowInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    m_Renderer->updateCamera(Renderer::CameraUpdateType::POSITION, glm::vec3(0.0f, 0.0f, static_cast<float>(yOffset)));
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
}

void cursorPositionCallback(GLFWwindow* window, double x, double y)
{
    float xpos = static_cast<float>(x);
    float ypos = static_cast<float>(y);

    float dx = xpos - m_LastX;
    float dy = ypos - m_LastY;

    m_LastX = xpos;
    m_LastY = ypos;

    if (m_LeftMouseButtonPressed)
        m_Renderer->updateCamera(Renderer::CameraUpdateType::ROTATION, glm::vec3(dy, dx, 0.0f));
    
    if (m_MiddleMouseButtonPressed)
        m_Renderer->updateCamera(Renderer::CameraUpdateType::POSITION, glm::vec3(dx * 0.005f, -dy * 0.005f, 0.0f));
}