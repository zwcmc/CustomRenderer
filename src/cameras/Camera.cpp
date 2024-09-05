#include "Camera.h"

#include <glad/glad.h>

Camera::Camera(const glm::vec3 &eye, const glm::vec3 &lookAt, const glm::vec3 &upVector)
    : m_EyePosition(eye), m_LookAt(lookAt), m_UpVector(upVector)
{
    m_IsPerspective = true;
    m_Projection = glm::mat4(1.0f);
    m_View = glm::mat4(1.0f);
    m_ScreenSize = glm::u32vec2(1);
    m_Fov = 60.0f;
    m_Aspect = 1.0f;
    m_ZNear = 0.001f;
    m_ZFar = 100.0f;
    
    m_View = glm::lookAt(eye, lookAt, upVector);
}

void Camera::SetPerspective(const float &fov, const int &width, const int &height, const float &zNear, const float &zFar)
{
    m_Fov = fov;
    m_ScreenSize.x = width;
    m_ScreenSize.y = height;
    m_Aspect = static_cast<float>(width) / height;
    m_ZNear = zNear;
    m_ZFar = zFar;
    
    m_Projection = glm::perspective(m_Fov, m_Aspect, m_ZNear, m_ZFar);
}

void Camera::SetOrthographic(const float &left, const float &right, const float &bottom, const float &top, const float &zNear, const float &zFar)
{
    m_IsPerspective = false;

    m_ZNear = zNear;
    m_ZFar = zFar;
    
    m_Projection = glm::ortho(left, right, bottom, top, zNear, zFar);
}

void Camera::SetScreenSize(const int &width, const int &height)
{
    m_ScreenSize.x = width;
    m_ScreenSize.y = height;

    if (m_IsPerspective)
    {
        m_Aspect = static_cast<float>(width) / height;
        m_Projection = glm::perspective(m_Fov, m_Aspect, m_ZNear, m_ZFar);
    }
}

glm::u32vec2& Camera::GetScreenSize()
{
    return m_ScreenSize;
}

glm::mat4& Camera::GetViewMatrix()
{
    return m_View;
}

glm::mat4& Camera::GetProjectionMatrix()
{
    return m_Projection;
}

glm::vec3& Camera::GetEyePosition()
{
    return m_EyePosition;
}

float& Camera::GetNear()
{
    return m_ZNear;
}

float& Camera::GetFar()
{
    return m_ZFar;
}

void Camera::BindCameraTarget()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_ScreenSize.x, m_ScreenSize.y);
}
