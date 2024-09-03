#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "ptr.h"
#include "renderer/RenderCommand.h"
#include "cameras/Camera.h"
#include "lights/DirectionalLight.h"
#include "base/Material.h"
#include "meshes/Mesh.h"
#include "scene/SceneNode.h"

using namespace glm;

class DirectionalLightShadowMap
{
    SHARED_PTR(DirectionalLightShadowMap)
public:
    DirectionalLightShadowMap();
    ~DirectionalLightShadowMap() = default;
    
    void SetCascadeShadowMapsEnabled(const bool &enabled);
    void RenderShadowMap(const Camera::Ptr viewCamera, const DirectionalLight::Ptr light, const std::vector<RenderCommand::Ptr> &shadowCasterCommands, const SceneNode::Ptr scene);
    
    void RenderShadowCasters(Mesh::Ptr mesh);
    
    void ComputeShadowProjectionFitViewFrustum(std::vector<vec3> &frustumPoints, const mat4 &cameraView, const mat4 &lightView, vec3 &lightCameraOrthographicMin, vec3 &lightCameraOrthographicMax);
    void RemoveShimmeringEdgeEffect(const std::vector<vec3> &frustumPoints, const int &bufferSize, vec3 &lightCameraOrthographicMin, vec3 &lightCameraOrthographicMax);
    void ComputeNearAndFar(float &nearPlane, float &farPlane, const vec3 &lightCameraOrthographicMin, const vec3 &lightCameraOrthographicMax, const std::vector<vec3> &sceneAABBPointsLightSpace);
    
    mat4& GetLightCameraView();
    std::vector<mat4>& GetShadowProjections();
    std::vector<vec4>& GetCascadeScalesAndOffsets();
    vec4& GetShadowCascadeParams();

private:
    struct Triangle
    {
        vec3 pt[3];
        bool culled;
    };

    // Percent of the cascade frustum
    static constexpr int CASCADE_PARTITION_PERCENTS[] =
    {
        5,
        15,
        60,
        100
    };
    static const int CASCADE_PARTITION_MAX = 100;
    static const int MAX_CASCADES = 4;
    
    static constexpr int iAABBTriIndexes[] =
    {
        0,1,2,  2,3,0,
        4,5,6,  6,7,4,
        0,1,5,  5,4,0,

        2,3,7,  7,6,2,
        0,3,7,  7,4,0,
        1,2,6,  6,5,1
    };

    std::vector<mat4> m_MatShadowProjections;
    std::vector<vec4> m_CascadeScalesAndOffsets;
    bool m_UseCascadeShadowMaps;
    Material::Ptr m_DirectionalShadowCasterMat;
    Camera::Ptr m_LightCamera;
    vec4 m_CascadeParams;
};
