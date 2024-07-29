#include "Camera.h"

Camera::Camera(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
    : m_ProjectionMatrix(projectionMatrix), m_ViewMatrix(viewMatrix), m_Position(glm::vec3(0.0f, 0.0f, -3.0f)), m_Rotation(glm::vec3(0.0f, 0.0f, 0.0f)),
        m_Fovy(glm::radians(45.0f)), m_Aspect(1.0f), m_ZNear(0.1f), m_ZFar(1000.0f)
{
    updateViewMatrix();
}

Camera::Camera(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& position, glm::vec3& rotation)
    : m_ProjectionMatrix(projectionMatrix), m_ViewMatrix(viewMatrix), m_Position(position), m_Rotation(rotation),
        m_Fovy(glm::radians(45.0f)), m_Aspect(1.0f), m_ZNear(0.1f), m_ZFar(1000.0f)
{
    updateViewMatrix();
}

Camera* Camera::perspectiveCamera(float fovy, float aspect, float zNear, float zFar)
{
    glm::mat4 projection = glm::perspective(fovy, aspect, zNear, zFar);
    glm::mat4 view(1.0f);

    Camera* camera = new Camera(projection, view);
    camera->setFovAspectZNearFar(fovy, aspect, zNear, zFar);
    return camera;
}

void Camera::setPosition(const glm::vec3& position)
{
    this->m_Position = position;
    updateViewMatrix();
}

void Camera::setRotation(const glm::vec3& rotation)
{
    this->m_Rotation = rotation;
    updateViewMatrix();
}

void Camera::translate(glm::vec3& offset)
{
    this->m_Position += offset;
    updateViewMatrix();
}

void Camera::rotate(glm::vec3& delta)
{
    this->m_Rotation += delta * ROTATION_SPEED;
    updateViewMatrix();
}

void Camera::setAspectRatio(float aspect)
{
    m_ProjectionMatrix = glm::perspective(m_Fovy, aspect, m_ZNear, m_ZFar);
}

glm::mat4& Camera::getViewMatrix()
{
    return m_ViewMatrix;
}

glm::mat4& Camera::getProjectionMatrix()
{
    return m_ProjectionMatrix;
}

void Camera::setFovAspectZNearFar(float fovy, float aspect, float zNear, float zFar)
{
    this->m_Fovy = fovy;
    this->m_Aspect = aspect;
    this->m_ZNear = zNear;
    this->m_ZFar = zFar;
}

void Camera::updateViewMatrix()
{
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    // Rotate by z-axis
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    // Rotate by x-axis
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    // Rotate by y-axis
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_Position);

    m_ViewMatrix = translationMatrix * rotationMatrix;
}