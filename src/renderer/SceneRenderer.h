#pragma once

#include <iostream>
#include <vector>

#include "renderer/Camera.h"
#include "renderer/ModelRenderer.h"
#include "lights/BaseLight.h"

#include "ptr.h"

class SceneRenderer
{
SHARED_PTR(SceneRenderer)
public:
    enum class CameraUpdateType
    {
        POSITION,
        ASPECT_RATIO
    };

    SceneRenderer();
    ~SceneRenderer();

    void setCamera(Camera::Ptr camera);
    void updateCamera(const CameraUpdateType &moveType, const glm::vec3 &delta = glm::vec3(0.0f));
    void rotateModelRenderers(const glm::vec3 &delta = glm::vec3(0.0f));

    void addModelRenderer(ModelRenderer::Ptr renderer);
    void addLight(BaseLight::Ptr light);

    void render();
private:
    Camera::Ptr m_Camera;
    std::vector<ModelRenderer::Ptr> m_ModelRenderers;
    std::vector<BaseLight::Ptr> m_Lights;
};