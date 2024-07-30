#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
    Camera(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix);
    Camera(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix, const glm::vec3 &position, glm::vec3 &rotation);
    ~Camera() = default;

    static Camera* perspectiveCamera(float fovy, float aspect, float zNear, float zFar);

    void setPosition(const glm::vec3 &position);
    void setRotation(const glm::vec3 &rotation);
    void translate(const glm::vec3 &offset);
    void rotate(const glm::vec3 &delta);
    void setAspectRatio(float aspect);

    glm::mat4& getViewMatrix();
    glm::mat4& getProjectionMatrix();

private:
    void setFovAspectZNearFar(float fovy, float aspect, float zNear, float zFar);
    void updateViewMatrix();

    const float ROTATION_SPEED = 0.1f;

    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;
    glm::vec3 m_Position, m_Rotation;

    float m_Fovy, m_Aspect, m_ZNear, m_ZFar;
};