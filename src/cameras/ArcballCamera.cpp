#include "cameras/ArcballCamera.h"

ArcballCamera::ArcballCamera(const glm::vec3 &eye, const glm::vec3 lookAt, const glm::vec3 upVector)
    : m_Position(eye), m_LookAt(lookAt), m_UpVector(upVector), m_ScreenWidth(0), m_ScreenHeight(0)
{
    updateViewMatrix();
}

ArcballCamera::Ptr ArcballCamera::perspectiveCamera(float fovy, float aspect, float zNear, float zFar)
{
    ArcballCamera::Ptr camera = ArcballCamera::New(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    camera->setFovAspectZNearFar(fovy, aspect, zNear, zFar);
    return camera;
}

void ArcballCamera::setScreenSize(const int &width, const int &height)
{
    m_ScreenWidth = width;
    m_ScreenHeight = height;
}

void ArcballCamera::setAspectRatio(float &aspect)
{
    m_Aspect = aspect;

    updateProjectionMatrix();
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

    float deltaAngleX = (2.0f * M_PI / m_ScreenWidth);
    float deltaAngleY = (M_PI / m_ScreenHeight);

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

void ArcballCamera::updateViewMatrix()
{
    m_ViewMatrix = glm::lookAt(m_Position, m_LookAt, m_UpVector);
}

void ArcballCamera::setFovAspectZNearFar(float fovy, float aspect, float zNear, float zFar)
{
    m_Fovy = fovy;
    m_Aspect = aspect;
    m_ZNear = zNear;
    m_ZFar = zFar;

    updateProjectionMatrix();
}

void ArcballCamera::updateProjectionMatrix()
{
    m_ProjectionMatrix = glm::perspective(m_Fovy, m_Aspect, m_ZNear, m_ZFar);
}