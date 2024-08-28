#include "renderer/DirectionalLightShadowMap.h"
#include "utility/Collision.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Collision;

DirectionalLightShadowMap::DirectionalLightShadowMap()
    : m_UseCascadeShadowMaps(false), m_CascadeParams(vec4(0.0f))
{
    m_DirectionalShadowCasterMat = Material::New("DirectionalShadowCaster", "glsl_shaders/ShadowCaster.vert", "glsl_shaders/ShadowCaster.frag");
    m_MatShadowProjections.resize(MAX_CASCADES, mat4(1.0f));
    m_CascadeScalesAndOffsets.resize(MAX_CASCADES, vec4(1.0f, 1.0f, 0.0f, 0.0f));
}

void DirectionalLightShadowMap::SetCascadeShadowMapsEnabled(const bool &enabled)
{
    m_UseCascadeShadowMaps = enabled;
}

void DirectionalLightShadowMap::RenderShadowMap(const Camera::Ptr viewCamera, const DirectionalLight::Ptr light, const std::vector<RenderCommand::Ptr> &shadowCasterCommands, const SceneNode::Ptr scene)
{
    m_LightCamera = Camera::New(light->GetLightPosition(), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 viewCameraProjection = viewCamera->GetProjectionMatrix();
    mat4 viewCameraView = viewCamera->GetViewMatrix();
    mat4 lightCameraView = m_LightCamera->GetViewMatrix();

    // Slope-Scale Depth Bias
    glEnable(GL_POLYGON_OFFSET_FILL);

    light->GetShadowMapRT()->Bind();
    m_DirectionalShadowCasterMat->Use();
    
    int cascadesCnt = m_UseCascadeShadowMaps ? MAX_CASCADES : 1;
    int shadowMapResolution = light->GetShadowMapRT()->GetSize().x;
    
    m_CascadeParams.x = static_cast<float>(cascadesCnt);
    m_CascadeParams.y = 1.0f / shadowMapResolution;
    m_CascadeParams.z = static_cast<float>(shadowMapResolution - 1) / shadowMapResolution;

    // FIT_TO_SCENE cascades
    float fCameraNearFarRange = viewCamera->GetFar() - viewCamera->GetNear();
    float fFrustumIntervalBegin = -viewCamera->GetNear();
    float fFrustumIntervalEnd = fCameraNearFarRange;

    for (int iCascadeIndex = 0; iCascadeIndex < cascadesCnt; ++iCascadeIndex)
    {
        // Different slope-scale depth bias for different cascade
        if (iCascadeIndex <= 0)
            glPolygonOffset(1.1f, 4.0f);
        else
            glPolygonOffset(2.2f, 4.0f);

        int cascadeResolution = m_UseCascadeShadowMaps ? shadowMapResolution >> 1 : shadowMapResolution;

        if (m_UseCascadeShadowMaps)
        {
            fFrustumIntervalEnd = static_cast<float>(CASCADE_PARTITION_PERCENTS[iCascadeIndex]);
            fFrustumIntervalEnd /= CASCADE_PARTITION_MAX;
            fFrustumIntervalEnd *= fCameraNearFarRange;
        }

        // Calculate a tight light camera projection to fit the camera view frustum
        // Calculate 8 corner points of view frustum first
        BoundingFrustum viewFrustum(viewCameraProjection);
        viewFrustum.Near = fFrustumIntervalBegin;
        viewFrustum.Far = -fFrustumIntervalEnd;

        std::vector<vec3> frustumPoints = viewFrustum.GetCorners();
        vec3 vLightCameraOrthographicMin = vec3(FLT_MAX);
        vec3 vLightCameraOrthographicMax = vec3(-FLT_MAX);
        ComputeShadowProjectionFitViewFrustum(frustumPoints, viewCameraView, lightCameraView, vLightCameraOrthographicMin, vLightCameraOrthographicMax);

        // Remove the shimmering edge effect along the edges of shadows due to the light changing to fit the camera by moving the light in texel-sized increments
        RemoveShimmeringEdgeEffect(frustumPoints, cascadeResolution, vLightCameraOrthographicMin, vLightCameraOrthographicMax);

        // Calculate the near and far plane
        BoundingBox bb = scene->AABB;
        std::vector<vec3> sceneAABBPoints = bb.GetCorners();
        // Transform the scene AABB to light space
        std::vector<vec3> sceneAABBPointsLightSpace;
        sceneAABBPointsLightSpace.resize(8);
        for (int index = 0; index < 8; ++index)
            sceneAABBPointsLightSpace[index] = make_vec3(lightCameraView * vec4(sceneAABBPoints[index], 1.0f));

        // Compute the near and far plane
        // Near and far plane are negative in OpenGL right-hand coordinate
        float nearPlane = 0.0f;
        float farPlane = 10000.0f;
        ComputeNearAndFar(nearPlane, farPlane, vLightCameraOrthographicMin, vLightCameraOrthographicMax, sceneAABBPointsLightSpace);

        // Shadow Pancaking
        if (vLightCameraOrthographicMax.z < nearPlane)
            nearPlane = vLightCameraOrthographicMax.z;

        // Create the tight orthographic projection for the light camera
        m_LightCamera->SetOrthographic(vLightCameraOrthographicMin.x, vLightCameraOrthographicMax.x, vLightCameraOrthographicMin.y, vLightCameraOrthographicMax.y, -nearPlane, -farPlane);

        m_MatShadowProjections[iCascadeIndex] = m_LightCamera->GetProjectionMatrix();

        int offsetX = (iCascadeIndex % 2) * cascadeResolution;
        int offsetY = (iCascadeIndex / 2) * cascadeResolution;
        glViewport(offsetX, offsetY, cascadeResolution, cascadeResolution);

        for (size_t i = 0; i < shadowCasterCommands.size(); ++i)
        {
            RenderCommand::Ptr command = shadowCasterCommands[i];
            m_DirectionalShadowCasterMat->SetMatrix("uLightMVP", m_MatShadowProjections[iCascadeIndex] * lightCameraView * command->Transform);
            RenderShadowCasters(command->Mesh);
        }

        // Apply cascade shadow transfom for shadow mapping, convert xyz from [-1, 1] to [0, 1]: xyz * 0.5 + 0.5.
        mat4 textureScaleAndBias = mat4(1.0f);
        // Scale
        textureScaleAndBias[0][0] = 0.5f;
        textureScaleAndBias[1][1] = 0.5f;
        textureScaleAndBias[2][2] = 0.5f;
        // Bias
        textureScaleAndBias[3][0] = 0.5f;
        textureScaleAndBias[3][1] = 0.5f;
        textureScaleAndBias[3][2] = 0.5f;
        m_MatShadowProjections[iCascadeIndex] = textureScaleAndBias * m_MatShadowProjections[iCascadeIndex];

        // Scales and offsets for mapping cascade texture coordinates.
        vec4 scaleAndOffset = vec4(0.0f);
        scaleAndOffset.x = m_UseCascadeShadowMaps ? 0.5f : 1.0f;
        scaleAndOffset.y = m_UseCascadeShadowMaps ? 0.5f : 1.0f;
        scaleAndOffset.z = static_cast<float>(offsetX) / shadowMapResolution;
        scaleAndOffset.w = static_cast<float>(offsetY) / shadowMapResolution;
        m_CascadeScalesAndOffsets[iCascadeIndex] = scaleAndOffset;
    }
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void DirectionalLightShadowMap::RenderShadowCasters(Mesh::Ptr mesh)
{
    glBindVertexArray(mesh->GetVertexArrayID());

    if (mesh->GetIndicesCount() > 0)
        glDrawElements(GL_TRIANGLES, mesh->GetIndicesCount(), GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(GL_TRIANGLES, 0, mesh->GetVerticesCount());

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

void DirectionalLightShadowMap::ComputeShadowProjectionFitViewFrustum(std::vector<vec3> &frustumPoints, const mat4 &cameraView, const mat4 &lightView,vec3 &lightCameraOrthographicMin, vec3 &lightCameraOrthographicMax)
{
    mat4 inverseCameraView = inverse(cameraView);
    
    vec3 tempTranslatedPoint;
    for (size_t i = 0; i < 8; ++i)
    {
        // Transform the frustum from camera view space to world space
        frustumPoints[i] = make_vec3(inverseCameraView * vec4(frustumPoints[i], 1.0f));
        // Transform the frustum from world space to light view space
        tempTranslatedPoint = make_vec3(lightView * vec4(frustumPoints[i], 1.0f));
        // Find the min and max
        lightCameraOrthographicMin = min(tempTranslatedPoint, lightCameraOrthographicMin);
        lightCameraOrthographicMax = max(tempTranslatedPoint, lightCameraOrthographicMax);
    }
}

void DirectionalLightShadowMap::RemoveShimmeringEdgeEffect(const std::vector<vec3> &frustumPoints, const int &bufferSize, vec3 &lightCameraOrthographicMin, vec3 &lightCameraOrthographicMax)
{
    vec3 vWorldUnitsPerTexel = vec3(0.0f);

    // Fit to the scene
    vec3 vDiagonal = frustumPoints[0] - frustumPoints[6];
    float fCascadeBound = length(vDiagonal);
    vec3 vBoarderOffset = (vec3(fCascadeBound) - (lightCameraOrthographicMax - lightCameraOrthographicMin)) * 0.5f;
    vBoarderOffset.z = 0.0f;
    lightCameraOrthographicMax += vBoarderOffset;
    lightCameraOrthographicMin -= vBoarderOffset;
    // The world units per texel are used to snap  the orthographic projection to texel sized increments.
    vWorldUnitsPerTexel = vec3(fCascadeBound / bufferSize, fCascadeBound / bufferSize, 1.0f);
    
//    // Fit to the cascade
//    int iPCFBlurSize = 2;
//    float fScaleDuetoBlureAMT = (float)(iPCFBlurSize * 2 + 1) / bufferSize;
//    vec3 normalizeByBufferSize = vec3(1.0f / bufferSize, 1.0f / bufferSize, 0.0f);
//    vec3 boardOffset = lightCameraOrthographicMax - lightCameraOrthographicMin;
//    boardOffset *= 0.5f;
//    boardOffset *= fScaleDuetoBlureAMT;
//    lightCameraOrthographicMax += boardOffset;
//    lightCameraOrthographicMin -= boardOffset;
//    // The world units per texel are used to snap  the orthographic projection to texel sized increments.
//    vWorldUnitsPerTexel = lightCameraOrthographicMax - lightCameraOrthographicMin;
//    vWorldUnitsPerTexel *= normalizeByBufferSize;
    
    lightCameraOrthographicMin /= vWorldUnitsPerTexel;
    lightCameraOrthographicMin = floor(lightCameraOrthographicMin);
    lightCameraOrthographicMin *= vWorldUnitsPerTexel;
    
    lightCameraOrthographicMax /= vWorldUnitsPerTexel;
    lightCameraOrthographicMax = floor(lightCameraOrthographicMax);
    lightCameraOrthographicMax *= vWorldUnitsPerTexel;
}

void DirectionalLightShadowMap::ComputeNearAndFar(float &nearPlane, float &farPlane, const vec3 &lightCameraOrthographicMin, const vec3 &lightCameraOrthographicMax, const std::vector<vec3> &sceneAABBPointsLightSpace)
{
    // Initialize the near and far planes
    // Right-hand coordinates in OpenGL, so all z coordinates are negative
    nearPlane = -FLT_MAX;
    farPlane = FLT_MAX;

    Triangle triangleList[16];
    int iTriangleCnt = 1;

    static const int iAABBTriIndexes[] =
    {
        0,1,2,  2,3,0,
        4,5,6,  6,7,4,
        0,1,5,  5,4,0,

        2,3,7,  7,6,2,
        0,3,7,  7,4,0,
        1,2,6,  6,5,1
    };

    int iPointPassesCollision[3];

    float fLightCameraOrthographicMinX = lightCameraOrthographicMin.x;
    float fLightCameraOrthographicMaxX = lightCameraOrthographicMax.x;
    float fLightCameraOrthographicMinY = lightCameraOrthographicMin.y;
    float fLightCameraOrthographicMaxY = lightCameraOrthographicMax.y;

    for (size_t AABBTriIter = 0; AABBTriIter < 12; ++AABBTriIter)
    {
        triangleList[0].pt[0] = sceneAABBPointsLightSpace[iAABBTriIndexes[AABBTriIter * 3 + 0]];
        triangleList[0].pt[1] = sceneAABBPointsLightSpace[iAABBTriIndexes[AABBTriIter * 3 + 1]];
        triangleList[0].pt[2] = sceneAABBPointsLightSpace[iAABBTriIndexes[AABBTriIter * 3 + 2]];
        iTriangleCnt = 1;
        triangleList[0].culled = false;

        for (size_t frustumPlaneIter = 0; frustumPlaneIter < 4; ++frustumPlaneIter)
        {
            float fEdge;
            int iComponent;

            if (frustumPlaneIter == 0)
            {
                fEdge = fLightCameraOrthographicMinX;
                iComponent = 0;
            }
            else if (frustumPlaneIter == 1)
            {
                fEdge = fLightCameraOrthographicMaxX;
                iComponent = 0;
            }
            else if (frustumPlaneIter == 2)
            {
                fEdge = fLightCameraOrthographicMinY;
                iComponent = 1;
            }
            else
            {
                fEdge = fLightCameraOrthographicMaxY;
                iComponent = 1;
            }

            for (size_t triIter = 0; triIter < iTriangleCnt; ++triIter)
            {
                if (!triangleList[triIter].culled)
                {
                    int iInsideVertCount = 0;
                    vec3 tempOrder;

                    if (frustumPlaneIter == 0)
                    {
                        for (size_t triPtIter = 0; triPtIter < 3; ++triPtIter)
                        {
                            if (triangleList[triIter].pt[triPtIter].x > lightCameraOrthographicMin.x)
                                iPointPassesCollision[triPtIter] = 1;
                            else
                                iPointPassesCollision[triPtIter] = 0;

                            iInsideVertCount += iPointPassesCollision[triPtIter];
                        }
                    }
                    else if (frustumPlaneIter == 1)
                    {
                        for (size_t triPtIter = 0; triPtIter < 3; ++triPtIter)
                        {
                            if (triangleList[triIter].pt[triPtIter].x < lightCameraOrthographicMax.x)
                                iPointPassesCollision[triPtIter] = 1;
                            else
                                iPointPassesCollision[triPtIter] = 0;

                            iInsideVertCount += iPointPassesCollision[triPtIter];
                        }
                    }
                    else if (frustumPlaneIter == 2)
                    {
                        for (size_t triPtIter = 0; triPtIter < 3; ++triPtIter)
                        {
                            if (triangleList[triIter].pt[triPtIter].y > fLightCameraOrthographicMinY)
                                iPointPassesCollision[triPtIter] = 1;
                            else
                                iPointPassesCollision[triPtIter] = 0;

                            iInsideVertCount += iPointPassesCollision[triPtIter];
                        }
                    }
                    else
                    {
                        for (size_t triPtIter = 0; triPtIter < 3; ++triPtIter)
                        {
                            if (triangleList[triIter].pt[triPtIter].y < fLightCameraOrthographicMaxY)
                                iPointPassesCollision[triPtIter] = 1;
                            else
                                iPointPassesCollision[triPtIter] = 0;

                            iInsideVertCount += iPointPassesCollision[triPtIter];
                        }
                    }

                    // Move the points that pass the frustum test to the begining of the array.
                    if (iPointPassesCollision[1] && !iPointPassesCollision[0])
                    {
                        tempOrder = triangleList[triIter].pt[0];
                        triangleList[triIter].pt[0] = triangleList[triIter].pt[1];
                        triangleList[triIter].pt[1] = tempOrder;
                        iPointPassesCollision[0] = 1;
                        iPointPassesCollision[1] = 0;
                    }
                    if (iPointPassesCollision[2] && !iPointPassesCollision[1])
                    {
                        tempOrder = triangleList[triIter].pt[1];
                        triangleList[triIter].pt[1] = triangleList[triIter].pt[2];
                        triangleList[triIter].pt[2] = tempOrder;
                        iPointPassesCollision[1] = 1;
                        iPointPassesCollision[2] = 0;
                    }
                    if (iPointPassesCollision[1] && !iPointPassesCollision[0])
                    {
                        tempOrder = triangleList[triIter].pt[0];
                        triangleList[triIter].pt[0] = triangleList[triIter].pt[1];
                        triangleList[triIter].pt[1] = tempOrder;
                        iPointPassesCollision[0] = 1;
                        iPointPassesCollision[1] = 0;
                    }

                    if (iInsideVertCount == 0)
                    {
                        triangleList[triIter].culled = true;
                    }
                    else if (iInsideVertCount == 1)
                    {
                        triangleList[triIter].culled = false;

                        vec3 vVert0ToVert1 = triangleList[triIter].pt[1] - triangleList[triIter].pt[0];
                        vec3 vVert0ToVert2 = triangleList[triIter].pt[2] - triangleList[triIter].pt[0];

                        float fHitPointTimeRatio = fEdge - (iComponent == 0 ? triangleList[triIter].pt[0].x : triangleList[triIter].pt[0].y);
                        float fDistanceAlongVector01 = fHitPointTimeRatio / (iComponent == 0 ? vVert0ToVert1.x : vVert0ToVert1.y);
                        float fDistanceAlongVector02 = fHitPointTimeRatio / (iComponent == 0 ? vVert0ToVert2.x : vVert0ToVert2.y);

                        vVert0ToVert1 *= fDistanceAlongVector01;
                        vVert0ToVert1 += triangleList[triIter].pt[0];
                        vVert0ToVert2 *= fDistanceAlongVector02;
                        vVert0ToVert2 += triangleList[triIter].pt[0];

                        triangleList[triIter].pt[1] = vVert0ToVert2;
                        triangleList[triIter].pt[2] = vVert0ToVert1;
                    }
                    else if (iInsideVertCount == 2)
                    {
                        triangleList[iTriangleCnt] = triangleList[triIter + 1];

                        triangleList[triIter].culled = false;
                        triangleList[triIter + 1].culled = false;

                        // Get the vector from the outside point into the 2 inside points.
                        vec3 vVert2ToVert0 = triangleList[triIter].pt[0] - triangleList[triIter].pt[2];
                        vec3 vVert2ToVert1 = triangleList[triIter].pt[1] = triangleList[triIter].pt[2];

                        // Get the hit point ratio
                        float fHitPointTime_2_0 = fEdge - (iComponent == 0 ? triangleList[triIter].pt[2].x : triangleList[triIter].pt[2].y);
                        float fDistanceAlongVector_2_0 = fHitPointTime_2_0 / (iComponent == 0 ? vVert2ToVert0.x : vVert2ToVert0.y);
                        vVert2ToVert0 *= fDistanceAlongVector_2_0;
                        vVert2ToVert0 += triangleList[triIter].pt[2];

                        triangleList[triIter + 1].pt[0] = triangleList[triIter].pt[0];
                        triangleList[triIter + 1].pt[1] = triangleList[triIter].pt[1];
                        triangleList[triIter + 1].pt[2] = vVert2ToVert0;

                        float fHitPointTime_2_1 = fEdge - (iComponent == 0 ? triangleList[triIter].pt[2].x : triangleList[triIter].pt[2].y);
                        float dDistanceAlongVector_2_1 = fHitPointTime_2_1 / (iComponent == 0 ? vVert2ToVert1.x : vVert2ToVert1.y);
                        vVert2ToVert1 *= dDistanceAlongVector_2_1;
                        vVert2ToVert1 += triangleList[triIter].pt[2];
                        triangleList[triIter].pt[0] = triangleList[triIter + 1].pt[1];
                        triangleList[triIter].pt[1] = triangleList[triIter + 1].pt[2];
                        triangleList[triIter].pt[2] = vVert2ToVert1;

                        ++iTriangleCnt;
                        ++triIter;
                    }
                    else
                    {
                        triangleList[triIter].culled = false;
                    }
                }
            }
        }

        for (size_t index = 0; index < iTriangleCnt; ++index)
        {
            if (!triangleList[index].culled)
            {
                for (size_t vertIndex = 0; vertIndex < 3; ++vertIndex)
                {
                    float fTriangleCoordZ = triangleList[index].pt[vertIndex].z;
                    if (nearPlane < fTriangleCoordZ)
                        nearPlane = fTriangleCoordZ;
                    if (farPlane > fTriangleCoordZ)
                        farPlane = fTriangleCoordZ;
                }
            }
        }
    }
}

glm::mat4& DirectionalLightShadowMap::GetLightCameraView()
{
    return m_LightCamera->GetViewMatrix();
}

std::vector<mat4>& DirectionalLightShadowMap::GetShadowProjections()
{
    return m_MatShadowProjections;
}

std::vector<vec4>& DirectionalLightShadowMap::GetCascadeScalesAndOffsets()
{
    return m_CascadeScalesAndOffsets;
}

vec4& DirectionalLightShadowMap::GetShadowCascadeParams()
{
    return m_CascadeParams;
}
