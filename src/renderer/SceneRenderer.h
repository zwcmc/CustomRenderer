#pragma once

#include <iostream>
#include <vector>

#include "renderer/Camera.h"
#include "renderer/ModelRenderer.h"

#include "ptr.h"

class SceneRenderer
{
SHARED_PTR(SceneRenderer)
public:
    enum class CameraUpdateType
    {
        POSITION,
        ROTATION,
        ASPECT_RATIO
    };

    SceneRenderer();
    ~SceneRenderer() = default;

    void setCamera(Camera::Ptr camera);
    void updateCamera(const CameraUpdateType &moveType, const glm::vec3 &delta = glm::vec3(0.0f));
    void addModelRenderer(ModelRenderer::Ptr renderer);

    void render();
private:
    Camera::Ptr m_Camera;
    std::vector<ModelRenderer::Ptr> m_ModelRenders;
    // std::vector<Light*> m_Lights;
};