#pragma once

#include "ptr.h"
#include "Camera.h"

class ArcballCamera : public Camera
{
    SHARED_PTR(ArcballCamera)
public:
    ArcballCamera(const glm::vec3 &eye, const glm::vec3 &lookAt, const glm::vec3 &upVector);

    virtual void zooming(const float &yoffset) override;
    virtual void panning(const float &xoffset, const float &yoffset) override;
    virtual void arcballing(const float &xoffset, const float &yoffset) override;

private:
    glm::vec3 getRightVector() const;
    glm::vec3 getViewDir() const;

    static constexpr float CAMERA_PANNING_SPEED = 0.005f;
    static constexpr float CAMERA_ARCBALLING_SPEED = 0.2f;
    static constexpr float CAMERA_ZOOMING_SPEED = 0.1f;
};
