#pragma once

#include <iostream>
#include <vector>

#include "renderer/Camera.h"
#include "renderer/MeshRender.h"

class Renderer
{
public:
    enum class CameraUpdateType
    {
        POSITION,
        ROTATION,
        ASPECT_RATIO
    };

    Renderer();
    ~Renderer() = default;

    void setCamera(Camera* camera);
    void updateCamera(const CameraUpdateType &moveType, const glm::vec3 &delta = glm::vec3(0.0f));
    void addMeshRender(MeshRender* render);
    void render();
private:
    Camera* m_Camera;
    std::vector<MeshRender*> m_MeshRenders;
    // std::vector<Light*> m_Lights;
};