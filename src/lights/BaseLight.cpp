#include "lights/BaseLight.h"

#include "meshes/Sphere.h"
#include "loader/AssetsLoader.h"
#include "base/Shader.h"
#include "renderer/ShapeRenderer.h"

BaseLight::BaseLight()
    : m_Position(glm::vec3(0.0f)), m_Intensity(1.0f), m_Color(glm::vec3(1.0f))
{
    initMeshRender();
}

BaseLight::BaseLight(const glm::vec3 &position, const glm::vec3 &color, const float &intensity)
{
    m_Position = position;
    m_Color = color;
    m_Intensity = intensity;

    initMeshRender();
}

void BaseLight::draw(ArcballCamera::Ptr camera)
{
    m_ShapeRenderer->draw(camera);
}

void BaseLight::initMeshRender()
{
    Shader::Ptr shader = AssetsLoader::loadShaderFromFile("RenderLight", "glsl_shaders/RenderLight.vs", "glsl_shaders/RenderLight.fs");
    Material::Ptr mat = Material::New(shader);
    mat->addVectorProperty("uLightColor", glm::vec4(m_Color, 1.0f));
    Sphere::Ptr sphere = Sphere::New(32, 32);
    m_ShapeRenderer = ShapeRenderer::New(sphere, mat);
    m_ShapeRenderer->translate(m_Position);
    m_ShapeRenderer->scale(glm::vec3(0.01f));
}