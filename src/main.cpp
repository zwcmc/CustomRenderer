#include <string>
#include <iostream>
#include <glad/glad.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

#include "cameras/ArcballCamera.h"

#include "lights/DirectionalLight.h"

#include "loader/AssetsLoader.h"

#include "scene/SceneNode.h"

#include "scene/SceneRenderGraph.h"

#include "utility/StatusRecorder.h"

//#include <iomanip>

const int WIDTH = 1280;
const int HEIGHT = 720;

float m_LastX = WIDTH / 2.0f;
float m_LastY = HEIGHT / 2.0f;
bool m_LeftMouseButtonPressed = false;
bool m_RightMouseButtonPressed = false;
bool m_MouseOverImGui = false;

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

    // cannot Use any features that have been marked as deprecated
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
    // Enable vsync
    glfwSwapInterval(1);

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
    m_SceneRenderGraph->Init();

//    SceneNode::Ptr glTFNode = AssetsLoader::LoadModel("models/glTF/DamagedHelmet/glTF/DamagedHelmet.gltf");
//    SceneNode::Ptr glTFNode = AssetsLoader::LoadModel("models/glTF/buster_drone/busterDrone.gltf");
//    SceneNode::Ptr glTFNode = AssetsLoader::LoadModel("models/glTF/DragonAttenuation/glTF/DragonAttenuation.gltf");
//    m_SceneRenderGraph->AddSceneNode(glTFNode);

//    SceneNode::Ptr marry = AssetsLoader::LoadModel("models/obj/mary/Marry.obj");
//    marry->Translate(glm::vec3(0.0f, -1.5f, 0.0f));
//    m_SceneRenderGraph->AddSceneNode(marry);
    
    SceneNode::Ptr marry2 = AssetsLoader::LoadModel("models/obj/mary/Marry.obj");
    marry2->Translate(glm::vec3(0.0f, -1.5f, 0.0f));
    m_SceneRenderGraph->AddSceneNode(marry2);
    
    SceneNode::Ptr bunny = AssetsLoader::LoadModel("models/obj/bunny/bunny.obj");
    bunny->Translate(glm::vec3(-2.0f, -2.1f, -1.0f));
    bunny->Scale(glm::vec3(16.0f));
    m_SceneRenderGraph->AddSceneNode(bunny);
    
    SceneNode::Ptr cube = AssetsLoader::LoadModel("models/obj/cube/cube.obj");
    cube->Translate(glm::vec3(2.0f, -1.0f, 0.0f));
    cube->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(30.0f));
    m_SceneRenderGraph->AddSceneNode(cube);
    
    SceneNode::Ptr floor = AssetsLoader::LoadModel("models/obj/floor/floor.obj");
    floor->Translate(glm::vec3(0.0f, -1.5f, 0.0f));
    m_SceneRenderGraph->AddSceneNode(floor);

    // Get the real size in pixels
    int realWidth, realHeight;
    glfwGetFramebufferSize(m_Window, &realWidth, &realHeight);
    glViewport(0, 0, realWidth, realHeight);
    
    ArcballCamera::Ptr arcballCamera = ArcballCamera::New(glm::vec3(0.0f, 0.0f, 5.5f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    arcballCamera->SetPerspective(glm::radians(60.0f), realWidth, realHeight, 0.1f, 100.0f);
//    arcballCamera->SetOrthographic(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
    m_SceneRenderGraph->SetCamera(arcballCamera);

    m_SceneRenderGraph->SetRenderSize(realWidth, realHeight);

    DirectionalLight::Ptr mainLight = DirectionalLight::New(glm::vec3(32.0f, 30.0f, 12.0f), glm::vec3(1.0f), true);
    m_SceneRenderGraph->SetMainLight(mainLight);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    ImGuiIO &io = ImGui::GetIO(); (void)io;

    while (!glfwWindowShouldClose(m_Window))
    {
        processWindowInput(m_Window);

        m_SceneRenderGraph->Render();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Status", (bool*)true);
            ImGui::Text("FPS: %.1f(%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

            ImGui::Text(StatusRecorder::DeferredRendering ? "Rendering Path: Deferred" : "Rendering Path: Forward");

            if (ImGui::TreeNode("Post-processing"))
            {
                if (ImGui::TreeNode("Bloom"))
                {
                    ImGui::Checkbox("Enable", &StatusRecorder::Bloom);
                    ImGui::SliderFloat("Intensity", &StatusRecorder::BloomIntensity, 0.0f, 1.0f);
                    ImGui::SliderFloat("Scatter", &StatusRecorder::BloomScatter, 0.0f, 1.0f);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Color Grading"))
                {
                    ImGui::Checkbox("ToneMapping", &StatusRecorder::ToneMapping);
                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }

            ImGui::Checkbox("FXAA", &StatusRecorder::FXAA);
            ImGui::Checkbox("SSAO", &StatusRecorder::SSAO);
        }
        ImGui::End();
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Check whether to dispatch mouse/keyboard to Dear ImGui or my application
        m_MouseOverImGui = io.WantCaptureMouse;
        
        checkOpenGLError();

        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }

    // Cleanup
    m_SceneRenderGraph->Cleanup();

    // ImGui Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void checkOpenGLError()
{
    GLenum err = glGetError();
    while (err != GL_NO_ERROR)
    {
        const char* error;
        switch (err)
        {
            case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
            case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE: error = "INVALID_VALUE"; break;
            case GL_OUT_OF_MEMORY: error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
            default: error = "UNKNOWN_ERROR"; break;
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
        m_SceneRenderGraph->SetRenderSize(width, height);
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
    if (m_MouseOverImGui)
    {
        return;
    }

    m_SceneRenderGraph->GetActiveCamera()->Zooming(static_cast<float>(yoffset));
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            m_LeftMouseButtonPressed = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_LeftMouseButtonPressed = false;
        }
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
    if (m_MouseOverImGui)
    {
        return;
    }

    float xpos = static_cast<float>(x);
    float ypos = static_cast<float>(y);

    float dx = m_LastX - xpos;
    float dy = m_LastY - ypos;

    m_LastX = xpos;
    m_LastY = ypos;

    if (m_LeftMouseButtonPressed)
    {
        m_SceneRenderGraph->GetActiveCamera()->Arcballing(dx, dy);
    }

    if (m_RightMouseButtonPressed)
    {
        m_SceneRenderGraph->GetActiveCamera()->Panning(dx, -dy);
    }
}
