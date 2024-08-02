#pragma once

#include <iostream>
#include <vector>

#include "cameras/ArcballCamera.h"
#include "renderer/ModelRenderer.h"
#include "lights/BaseLight.h"

#include "ptr.h"

class SceneRenderer
{
    SHARED_PTR(SceneRenderer)
public:
    SceneRenderer();
    ~SceneRenderer();

    void setCamera(ArcballCamera::Ptr camera);
    ArcballCamera::Ptr getActiveCamera() { return m_Camera; }

    void rotateModelRenderers(const glm::vec3 &delta = glm::vec3(0.0f));

    void addModelRenderer(ModelRenderer::Ptr renderer);
    void addLight(BaseLight::Ptr light);

    void render();
private:
    ArcballCamera::Ptr m_Camera;
    std::vector<ModelRenderer::Ptr> m_ModelRenderers;
    std::vector<BaseLight::Ptr> m_Lights;

    static constexpr float ROTATION_SPEED = 0.05f;
};