#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ptr.h"

class Camera
{
    SHARED_PTR(Camera)
public:
    Camera(const glm::vec3 &eye, const glm::vec3 &lookAt, const glm::vec3 &upVector);
    virtual ~Camera() = default;
    
    void SetPerspective(const float &fov, const int &width, const int &height, const float &zNear, const float &zFar);
    void SetOrthographic(const float &left, const float &right, const float &bottom, const float &top, const float &zNear, const float &zFar);

    void SetScreenSize(const int &width, const int &height);
    
    glm::mat4& GetViewMatrix();
    glm::mat4& GetProjectionMatrix();
    glm::vec3& GetEyePosition();
    float& GetNear();
    float& GetFar();

    virtual void Zooming(const float &yoffset) { }
    virtual void Panning(const float &xoffset, const float &yoffset) { }
    virtual void Arcballing(const float &xoffset, const float &yoffset) { }

protected:
    bool m_IsPerspective;
    glm::mat4 m_Projection, m_View;
    glm::vec3 m_EyePosition, m_LookAt, m_UpVector;
    glm::u32vec2 m_ScreenSize;
    float m_Fov, m_Aspect, m_ZNear, m_ZFar;
};
