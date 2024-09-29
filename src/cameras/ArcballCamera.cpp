#include "cameras/ArcballCamera.h"

#include "defines.h"

ArcballCamera::ArcballCamera(const glm::vec3 &eye, const glm::vec3 &lookAt, const glm::vec3 &upVector)
    : Camera(eye, lookAt, upVector)
{ }

void ArcballCamera::Zooming(const float &yoffset)
{
    if (m_IsPerspective)
    {
        m_Fov -= yoffset * CAMERA_ZOOMING_SPEED;
        m_Fov = glm::clamp(m_Fov, glm::radians(1.0f), glm::radians(90.0f));
        m_Projection = glm::perspective(m_Fov, m_Aspect, m_ZNear, m_ZFar);
    }
}

void ArcballCamera::Panning(const float &xoffset, const float &yoffset)
{
    glm::vec3 cameraFront = glm::normalize(m_LookAt - m_EyePosition);
    glm::vec3 right = glm::normalize(glm::cross(cameraFront, m_UpVector));
    glm::vec3 up = glm::normalize(m_UpVector);

    m_EyePosition -= (xoffset * right + yoffset * up) * CAMERA_PANNING_SPEED;
    m_LookAt -= (xoffset * right + yoffset * up) * CAMERA_PANNING_SPEED;

    m_View = glm::lookAt(m_EyePosition, m_LookAt, m_UpVector);
}

void ArcballCamera::Arcballing(const float &xoffset, const float &yoffset)
{
    glm::vec4 position = glm::vec4(m_EyePosition, 1.0f);
    glm::vec4 pivot = glm::vec4(m_LookAt, 1.0f);

    float deltaAngleX = ((float)M_TAU / m_ScreenSize.x);
    float deltaAngleY = ((float)M_PI / m_ScreenSize.y);

    float xAngle = CAMERA_ARCBALLING_SPEED * xoffset * deltaAngleX;
    float yAngle = CAMERA_ARCBALLING_SPEED * yoffset * deltaAngleY;

    float cosAngle = glm::dot(GetViewDir(), m_UpVector);
    if (cosAngle * glm::sign(yAngle) > 0.99f)
        yAngle = 0.0f;

    // Rotate the camera around the target point on the up vector
    glm::mat4x4 rotationMatrixX(1.0f);
    rotationMatrixX = glm::rotate(rotationMatrixX, xAngle, m_UpVector);
    position = (rotationMatrixX * (position - pivot)) + pivot;

    // Rotate the camera around the target point on the right vector
    glm::mat4x4 rotationMatrixY(1.0f);
    rotationMatrixY = glm::rotate(rotationMatrixY, yAngle, GetRightVector());
    glm::vec3 finalPosition = (rotationMatrixY * (position - pivot)) + pivot;

    m_EyePosition = finalPosition;

    m_View = glm::lookAt(m_EyePosition, m_LookAt, m_UpVector);
}

glm::vec3 ArcballCamera::GetRightVector() const
{
    return glm::transpose(m_View)[0];
}

glm::vec3 ArcballCamera::GetViewDir() const
{
    return -glm::transpose(m_View)[2];
};
