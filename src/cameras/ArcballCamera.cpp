#include "cameras/ArcballCamera.h"

#include "globals.h"

ArcballCamera::ArcballCamera(const glm::vec3 &eye, const glm::vec3 lookAt, const glm::vec3 upVector)
    : m_Position(eye), m_LookAt(lookAt), m_UpVector(upVector), m_ScreenSize(glm::u32vec2(1))
{
    updateViewMatrix();
}

ArcballCamera::Ptr ArcballCamera::perspectiveCamera(float fovy, glm::u32vec2 size, float zNear, float zFar)
{
    ArcballCamera::Ptr camera = ArcballCamera::New(glm::vec3(0.0f, 0.0f, 3.5f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    camera->initCamera(fovy, size, zNear, zFar);
    return camera;
}

void ArcballCamera::setScreenSize(const glm::u32vec2& size)
{
    if (size.x != m_ScreenSize.x || size.y != m_ScreenSize.y)
    {
        m_ScreenSize = size;

        m_Aspect = static_cast<float>(size.x) / size.y;
        updateProjectionMatrix();
    }
}

void ArcballCamera::zooming(const float &yoffset)
{
    m_Fovy -= yoffset * CAMERA_ZOOMING_SPEED;
    m_Fovy = glm::clamp(m_Fovy, glm::radians(1.0f), glm::radians(90.0f));

    updateProjectionMatrix();
}

void ArcballCamera::panning(float xoffset, float yoffset)
{
    xoffset *= CAMERA_PANNING_SPEED;
    yoffset *= CAMERA_PANNING_SPEED;

    glm::vec3 cameraFront = glm::normalize(m_LookAt - m_Position);
    glm::vec3 right = glm::normalize(glm::cross(cameraFront, m_UpVector));
    glm::vec3 up = glm::normalize(m_UpVector);

    m_Position -= xoffset * right + yoffset * up;
    m_LookAt -= xoffset * right + yoffset * up;
    updateViewMatrix();
}

void ArcballCamera::arcballing(float xoffset, float yoffset)
{
    xoffset *= CAMERA_ARCBALLING_SPEED;
    yoffset *= CAMERA_ARCBALLING_SPEED;

    glm::vec4 position = glm::vec4(m_Position, 1.0f);
    glm::vec4 pivot = glm::vec4(m_LookAt, 1.0f);

    float deltaAngleX = (M_TAU / m_ScreenSize.x);
    float deltaAngleY = (M_PI / m_ScreenSize.y);

    float xAngle = xoffset * deltaAngleX;
    float yAngle = yoffset * deltaAngleY;

    float cosAngle = glm::dot(getViewDir(), m_UpVector);
    if (cosAngle * glm::sign(yAngle) > 0.99f)
        yAngle = 0.0f;

    // Rotate the camera around the target point on the up vector
    glm::mat4x4 rotationMatrixX(1.0f);
    rotationMatrixX = glm::rotate(rotationMatrixX, xAngle, m_UpVector);
    position = (rotationMatrixX * (position - pivot)) + pivot;

    // Rotate the camera around the target point on the right vector
    glm::mat4x4 rotationMatrixY(1.0f);
    rotationMatrixY = glm::rotate(rotationMatrixY, yAngle, getRightVector());
    glm::vec3 finalPosition = (rotationMatrixY * (position - pivot)) + pivot;

    m_Position = finalPosition;

    updateViewMatrix();
}

glm::vec3 ArcballCamera::getRightVector() const
{
    return glm::transpose(m_ViewMatrix)[0];
}

glm::vec3 ArcballCamera::getViewDir() const
{
    return -glm::transpose(m_ViewMatrix)[2];
};

void ArcballCamera::updateViewMatrix()
{
    m_ViewMatrix = glm::lookAt(m_Position, m_LookAt, m_UpVector);
}

void ArcballCamera::initCamera(float fovy, glm::u32vec2 size, float zNear, float zFar)
{
    m_Fovy = fovy;
    m_ScreenSize = size;
    m_Aspect = static_cast<float>(size.x) / size.y;
    m_ZNear = zNear;
    m_ZFar = zFar;

    updateProjectionMatrix();
}

void ArcballCamera::updateProjectionMatrix()
{
    m_ProjectionMatrix = glm::perspective(m_Fovy, m_Aspect, m_ZNear, m_ZFar);
}