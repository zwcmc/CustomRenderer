#pragma once

#include <iostream>
#include <vector>

#include "renderer/Camera.h"
#include "renderer/MeshRender.h"

#include "ptr.h"

class Renderer
{
    SHARED_PTR(Renderer)
public:
    enum class CameraUpdateType
    {
        POSITION,
        ROTATION,
        ASPECT_RATIO
    };

    Renderer();
    ~Renderer() = default;

    void setCamera(Camera::Ptr camera);
    void updateCamera(const CameraUpdateType &moveType, const glm::vec3 &delta = glm::vec3(0.0f));
    void addMeshRender(MeshRender::Ptr render);

    void render();
private:
    Camera::Ptr m_Camera;
    std::vector<MeshRender::Ptr> m_MeshRenders;
    // std::vector<Light*> m_Lights;
};