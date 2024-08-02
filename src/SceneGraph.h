#pragma once

#include <iostream>
#include <vector>

#include <glad/glad.h>

#include "ptr.h"

#include "cameras/ArcballCamera.h"
#include "lights/BaseLight.h"
#include "renderer/ModelRenderer.h"

class SceneGraph
{
    SHARED_PTR(SceneGraph)
public:
    SceneGraph();
    ~SceneGraph();

    void setCamera(ArcballCamera::Ptr camera);
    ArcballCamera::Ptr getActiveCamera() { return m_Camera; }

    void rotateModelRenderers(const glm::vec3 &delta = glm::vec3(0.0f));

    void addModelRenderer(ModelRenderer::Ptr renderer);
    void addLight(BaseLight::Ptr light);

    void render();

private:
    void init();

    ArcballCamera::Ptr m_Camera;
    std::vector<ModelRenderer::Ptr> m_ModelRenderers;
    std::vector<BaseLight::Ptr> m_Lights;

    GLuint m_GlobalUniformBufferID;

    static constexpr float ROTATION_SPEED = 0.05f;
};