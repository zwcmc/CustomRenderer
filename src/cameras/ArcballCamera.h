#pragma once

#include "ptr.h"
#include "Camera.h"

class ArcballCamera : public Camera
{
    SHARED_PTR(ArcballCamera)
public:
    ArcballCamera(const glm::vec3 &eye, const glm::vec3 &lookAt, const glm::vec3 &upVector);

    virtual void Zooming(const float &yoffset) override;
    virtual void Panning(const float &xoffset, const float &yoffset) override;
    virtual void Arcballing(const float &xoffset, const float &yoffset) override;

private:
    glm::vec3 GetRightVector() const;
    glm::vec3 GetViewDir() const;

    static constexpr float CAMERA_PANNING_SPEED = 0.005f;
    static constexpr float CAMERA_ARCBALLING_SPEED = 0.2f;
    static constexpr float CAMERA_ZOOMING_SPEED = 0.1f;
};
