#pragma once

#include "ptr.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ArcballCamera
{
    SHARED_PTR(ArcballCamera)
public:
    ArcballCamera() = default;
    ArcballCamera(const glm::vec3 &eye, const glm::vec3 lookAt, const glm::vec3 upVector);

    static ArcballCamera::Ptr perspectiveCamera(float fovy, float aspect, float zNear, float zFar);

    void setScreenSize(const int &width, const int &height);
    void setAspectRatio(float &aspect);

    void zooming(const float &yoffset);
    void panning(float xoffset, float yoffset);
    void arcballing(float xoffset, float yoffset);

    glm::mat4 &getViewMatrix() { return m_ViewMatrix; }
    glm::mat4 &getProjectionMatrix() { return m_ProjectionMatrix; }
    glm::vec3 &getPosition() { return m_Position; }

private:
    glm::vec3 getRightVector() const { return glm::transpose(m_ViewMatrix)[0]; }
    glm::vec3 getViewDir() const { return -glm::transpose(m_ViewMatrix)[2]; };

    void updateViewMatrix();
    void updateProjectionMatrix();
    void setFovAspectZNearFar(float fovy, float aspect, float zNear, float zFar);

    glm::mat4 m_ViewMatrix, m_ProjectionMatrix;
    glm::vec3 m_Position, m_LookAt, m_UpVector;
    float m_Fovy, m_Aspect, m_ZNear, m_ZFar;

    int m_ScreenWidth, m_ScreenHeight;

    static constexpr float CAMERA_PANNING_SPEED = 0.005f;
    static constexpr float CAMERA_ARCBALLING_SPEED = 0.2f;
    static constexpr float CAMERA_ZOOMING_SPEED = 0.1f;
};