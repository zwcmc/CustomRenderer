#include "scene/SceneRenderGraph.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "defines.h"
#include "meshes/AABBCube.h"
#include "utility/Collision.h"
#include "renderer/Blitter.h"

using namespace Collision;

SceneRenderGraph::SceneRenderGraph()
    : m_GlobalUniformBufferID(0), m_CullFace(true), m_Blend(false), m_RenderSize(glm::u32vec2(1))
{ }

SceneRenderGraph::~SceneRenderGraph()
{
    m_Lights.clear();
}

void SceneRenderGraph::Init()
{
    m_Scene = SceneNode::New();

    m_CommandBuffer = CommandBuffer::New();

    // No seams at cubemap edges
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // Set clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Depth test
    glEnable(GL_DEPTH_TEST);

    // Default cull face state
    m_CullFace = true;
    glEnable(GL_CULL_FACE);

    // Default blend state
    m_Blend = false;
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Global uniform buffer object
    glGenBuffers(1, &m_GlobalUniformBufferID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_GlobalUniformBufferID);
    glBufferData(GL_UNIFORM_BUFFER, 496, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_GlobalUniformBufferID); // Set global uniform to binding point 0
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Mesh for rendering lights
    m_LightMesh = Sphere::New(16, 16, 0.02f);

    // Init Blitter
    Blitter::Init();

    m_IntermediateRT = RenderTarget::New(1, 1, GL_HALF_FLOAT, 1, true);

    // Environment IBL
    m_EnvIBL = EnvironmentIBL::New("textures/environments/ktx/papermill.ktx", m_GlobalUniformBufferID);
    // Add render the skybox commands
    BuildSkyboxRenderCommands();

    // Main light shadowmap
    m_ShadowmapRT = RenderTarget::New(2048, 2048, GL_FLOAT, 0, false, true);
    m_ShadowCasterMat = Material::New("ShadowCaster", "glsl_shaders/ShadowCaster.vert", "glsl_shaders/ShadowCaster.frag");

    m_DebuggingAABBMat = Material::New("Draw AABB", "glsl_shaders/utils/DrawBoundingBox.vert", "glsl_shaders/utils/DrawBoundingBox.frag");
    m_DebuggingAABBMat->SetDoubleSided(true);
}

void SceneRenderGraph::Cleanup()
{
    // Cleanup Blitter
    Blitter::Cleanup();
}

void SceneRenderGraph::SetRenderSize(const int &width, const int &height)
{
    m_RenderSize.x = width;
    m_RenderSize.y = height;

    m_Camera->SetScreenSize(width, height);

    m_IntermediateRT->SetSize(glm::u32vec2(width, height));
}

void SceneRenderGraph::SetCamera(Camera::Ptr camera)
{
    m_Camera = camera;
}

void SceneRenderGraph::AddLight(Light::Ptr light)
{
    m_Lights.push_back(light);

    // Add a new render command for render light
    AddRenderLightCommand(light);
}

void SceneRenderGraph::AddSceneNode(SceneNode::Ptr sceneNode)
{
    m_Scene->AddChild(sceneNode);

    // Build render commands
    BuildRenderCommands(sceneNode);
}

void SceneRenderGraph::AddRenderLightCommand(Light::Ptr light)
{
    // TODO: All lights can share a single material
    Material::Ptr lightMat = Material::New("Emissive", "glsl_shaders/Emissive.vert", "glsl_shaders/Emissive.frag");
    lightMat->SetCastShadows(false);
    lightMat->AddOrSetVector("uEmissiveColor", light->GetLightColor());

    // Only need to modify the translation column
    glm::mat4 transform = glm::mat4(1.0f);
    glm::vec3 lightPos = light->GetLightPosition();
    transform[3][0] = lightPos.x;
    transform[3][1] = lightPos.y;
    transform[3][2] = lightPos.z;
    m_CommandBuffer->PushCommand(m_LightMesh, lightMat, transform);
}

void SceneRenderGraph::BuildSkyboxRenderCommands()
{
    BuildRenderCommands(m_EnvIBL->GetSkyboxRenderNode());
}

void SceneRenderGraph::BuildRenderCommands(SceneNode::Ptr sceneNode)
{
    glm::mat4 model = sceneNode->GetModelMatrix();
    Material::Ptr overrideMat = sceneNode->OverrideMat;
    for (size_t i = 0; i < sceneNode->MeshRenders.size(); ++i)
        m_CommandBuffer->PushCommand(sceneNode->MeshRenders[i]->GetMesh(), overrideMat ? overrideMat : sceneNode->MeshRenders[i]->GetMaterial(), model);

    // Debugging AABB
    if (sceneNode->IsAABBCalculated && false)
        m_CommandBuffer->PushDebuggingCommand(AABBCube::New(sceneNode->AABB.GetCorners()), m_DebuggingAABBMat, model);

    for (size_t i = 0; i < sceneNode->GetChildrenCount(); ++i)
        BuildRenderCommands(sceneNode->GetChildByIndex(i));
}

void SceneRenderGraph::CalculateSceneAABB()
{
    m_Scene->MergeChildrenAABBs(m_Scene->AABB);
    m_Scene->IsAABBCalculated = true;

    // Debugging camera's frustum
//    BoundingFrustum bf;
//    BoundingFrustum::CreateFromMatrix(bf, m_Camera->GetProjectionMatrix());
//    glm::mat4 tr = glm::mat4(1.0f);
//    tr = glm::translate(tr, m_Camera->GetEyePosition());
//    m_CommandBuffer->PushDebuggingCommand(AABBCube::New(bf.GetCorners()), m_DebuggingAABBMat, tr);

    // Debugging AABB
    // m_CommandBuffer->PushDebuggingCommand(AABBCube::New(m_Scene->AABB.GetCorners()), m_DebuggingAABBMat, glm::mat4(1.0f));
}

void SceneRenderGraph::ComputeShadowProjectionFitViewFrustum(std::vector<vec3> &frustumPoints, const glm::mat4 &cameraView, const glm::mat4 &lightView,vec3 &lightCameraOrthographicMin, vec3 &lightCameraOrthographicMax)
{
    glm::mat4 inverseCameraView = glm::inverse(cameraView);
    
    vec3 tempTranslatedPoint;
    for (size_t i = 0; i < 8; ++i)
    {
        // Transform the frustum from camera view space to world space
        frustumPoints[i] = glm::make_vec3(inverseCameraView * glm::vec4(frustumPoints[i], 1.0f));
        // Transform the frustum from world space to light view space
        tempTranslatedPoint = glm::make_vec3(lightView * glm::vec4(frustumPoints[i], 1.0f));
        // Find the min and max
        lightCameraOrthographicMin = glm::min(tempTranslatedPoint, lightCameraOrthographicMin);
        lightCameraOrthographicMax = glm::max(tempTranslatedPoint, lightCameraOrthographicMax);
    }
}

void SceneRenderGraph::RemoveShimmeringEdgeEffect(const std::vector<vec3> &frustumPoints, const glm::u32vec2 &bufferSize, vec3 &lightCameraOrthographicMin, vec3 &lightCameraOrthographicMax)
{
    vec3 vWorldUnitsPerTexel = vec3(0.0f);

    // Fit to the scene
    vec3 vDiagonal = frustumPoints[0] - frustumPoints[6];
    float fCascadeBound = glm::length(vDiagonal);
    vec3 vBoarderOffset = (vec3(fCascadeBound) - (lightCameraOrthographicMax - lightCameraOrthographicMin)) * 0.5f;
    vBoarderOffset.z = 0.0f;
    lightCameraOrthographicMax += vBoarderOffset;
    lightCameraOrthographicMin -= vBoarderOffset;
    // The world units per texel are used to snap  the orthographic projection to texel sized increments.
    vWorldUnitsPerTexel = vec3(fCascadeBound / bufferSize.x, fCascadeBound / bufferSize.y, 1.0f);
    
//    // Fit to the cascade
//    int iPCFBlurSize = 2;
//    float fScaleDuetoBlureAMT = (float)(iPCFBlurSize * 2 + 1) / bufferSize.x;
//    vec3 normalizeByBufferSize = vec3(1.0f / bufferSize.x, 1.0f / bufferSize.y, 0.0f);
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

void SceneRenderGraph::ComputeNearAndFar(float &nearPlane, float &farPlane, const vec3 &lightCameraOrthographicMin, const vec3 &lightCameraOrthographicMax, const std::vector<vec3> &sceneAABBPointsLightSpace)
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

void SceneRenderGraph::ExecuteCommandBuffer()
{
    Light::Ptr mainLight = m_Lights[0];
    Camera::Ptr lightCamera = Camera::New(mainLight->GetLightPosition(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 viewCameraProjection = m_Camera->GetProjectionMatrix();
    glm::mat4 viewCameraView = m_Camera->GetViewMatrix();
    glm::mat4 lightCameraView = lightCamera->GetViewMatrix();

    // Render shadowmap first
    // Percent of the cascade frustum
    float cascadePartitionPercents[] =
    {
        0.05f,
        0.15f,
        0.6f,
        1.0f
    };
    const int MAX_CASCADES = 4;
    glm::mat4 matShadowProjs[MAX_CASCADES];

    // FIT_TO_SCENE cascades
    float fCameraNearFarRange = m_Camera->GetFar() - m_Camera->GetNear();
    float fFrustumIntervalBegin = -m_Camera->GetNear();
    float fFrustumIntervalEnd;

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.1f, 4.0f);
    m_ShadowmapRT->Bind();
    m_ShadowCasterMat->Use();
    glm::u32vec2 shadowmapSize = m_ShadowmapRT->GetSize();
    for (int iCascadeIndex = 0; iCascadeIndex < MAX_CASCADES; ++iCascadeIndex)
    {
        int resolution = shadowmapSize.x >> 1;

        fFrustumIntervalEnd = cascadePartitionPercents[iCascadeIndex];
        fFrustumIntervalEnd *= fCameraNearFarRange;

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
        RemoveShimmeringEdgeEffect(frustumPoints, glm::u32vec2(resolution, resolution), vLightCameraOrthographicMin, vLightCameraOrthographicMax);

        // Calculate the near and far plane
        BoundingBox bb = m_Scene->AABB;
        std::vector<vec3> sceneAABBPoints = bb.GetCorners();
        // Transform the scene AABB to light space
        std::vector<vec3> sceneAABBPointsLightSpace;
        sceneAABBPointsLightSpace.resize(8);
        for (int index = 0; index < 8; ++index)
            sceneAABBPointsLightSpace[index] = glm::make_vec3(lightCameraView * glm::vec4(sceneAABBPoints[index], 1.0f));

        // Compute the near and far plane
        // Near and far plane are negative in OpenGL right-hand coordinate
        float nearPlane = 0.0f;
        float farPlane = 10000.0f;
        ComputeNearAndFar(nearPlane, farPlane, vLightCameraOrthographicMin, vLightCameraOrthographicMax, sceneAABBPointsLightSpace);

        // Shadow Pancaking
        if (vLightCameraOrthographicMax.z < nearPlane)
            nearPlane = vLightCameraOrthographicMax.z;

        // Create the tight orthographic projection for the light camera
        lightCamera->SetOrthographic(vLightCameraOrthographicMin.x, vLightCameraOrthographicMax.x, vLightCameraOrthographicMin.y, vLightCameraOrthographicMax.y, -nearPlane, -farPlane);

        matShadowProjs[iCascadeIndex] = lightCamera->GetProjectionMatrix();

        int offsetX = (iCascadeIndex % 2) * resolution;
        int offsetY = (iCascadeIndex / 2) * resolution;
        glViewport(offsetX, offsetY, resolution, resolution);

        std::vector<RenderCommand::Ptr> shadowCasterCommands = m_CommandBuffer->GetShadowCasterCommands();
        for (size_t i = 0; i < shadowCasterCommands.size(); ++i)
        {
            RenderCommand::Ptr command = shadowCasterCommands[i];
            m_ShadowCasterMat->SetMatrix("uLightMVP", matShadowProjs[iCascadeIndex] * lightCameraView * command->Transform);
            RenderMesh(command->Mesh);
        }

        // Apply cascade shadow transfom for shadow mapping, convert xyz from [-1, 1] to [0, 1]: xyz * 0.5 + 0.5.
        glm::mat4 textureScaleAndBias = glm::mat4(1.0f);
        // Scale
        textureScaleAndBias[0][0] = 0.5f;
        textureScaleAndBias[1][1] = 0.5f;
        textureScaleAndBias[2][2] = 0.5f;
        // Bias
        textureScaleAndBias[3][0] = 0.5f;
        textureScaleAndBias[3][1] = 0.5f;
        textureScaleAndBias[3][2] = 0.5f;
        matShadowProjs[iCascadeIndex] = textureScaleAndBias * matShadowProjs[iCascadeIndex];

        //// Cascade offset
        //glm::mat4 cascadeTransform = glm::mat4(1.0f);
        //float normalizeWidth = 1.0f / shadowmapSize.x;
        //float normalzeHeight = 1.0f / shadowmapSize.y;
        //cascadeTransform[0][0] = resolution * normalizeWidth;
        //cascadeTransform[1][1] = resolution * normalzeHeight;
        //cascadeTransform[3][0] = offsetX * normalizeWidth;
        //cascadeTransform[3][1] = offsetY * normalzeHeight;
        //matShadowProjs[iCascadeIndex] = cascadeTransform * matShadowProjs[iCascadeIndex];
    }
    glDisable(GL_POLYGON_OFFSET_FILL);

    // Set global uniforms
    glBindBuffer(GL_UNIFORM_BUFFER, m_GlobalUniformBufferID);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &(viewCameraView[0].x));
    glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &(viewCameraProjection[0].x));
    glBufferSubData(GL_UNIFORM_BUFFER, 128, 16, &(mainLight->GetLightPosition().x));
    glBufferSubData(GL_UNIFORM_BUFFER, 144, 16, &(mainLight->GetLightColor().x));
    glBufferSubData(GL_UNIFORM_BUFFER, 160, 16, &(m_Camera->GetEyePosition().x));
    glBufferSubData(GL_UNIFORM_BUFFER, 176, 256, &(matShadowProjs[0][0].x));
    glBufferSubData(GL_UNIFORM_BUFFER, 432, 64, &(lightCameraView[0].x));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    Blitter::BlitToCamera(m_ShadowmapRT->GetShadowmapTexture(), m_RenderSize); return;

    // Bind intermediate framebuffer
    m_IntermediateRT->Bind();

    // Opaque
    std::vector<RenderCommand::Ptr> opaqueCommands = m_CommandBuffer->GetOpaqueCommands();
    for (size_t i = 0; i < opaqueCommands.size(); ++i)
    {
       RenderCommand::Ptr command = opaqueCommands[i];
       RenderCommand(command);
    }

    // Skybox start ----------------
    // Skybox's depth always is 1.0, is equal to the max depth buffer, rendering skybox after opauqe objects and setting depth func to less&equal will
    // ensure that the skybox is only renderered in pixels that are not covered by the opaque objects.
    // Pixels covered by opaque objects have a depth less than 1.0. Therefore, the depth test will never pass when rendering the skybox.
    glDepthFunc(GL_LEQUAL);
    // Depth write off
    glDepthMask(GL_FALSE);
    std::vector<RenderCommand::Ptr> skyboxCommands = m_CommandBuffer->GetSkyboxCommands();
    for (size_t i = 0; i < skyboxCommands.size(); ++i)
    {
        RenderCommand::Ptr command = skyboxCommands[i];
        RenderCommand(command);
    }
    // Depth write on
    glDepthMask(GL_TRUE);
    // Set back to less
    glDepthFunc(GL_LESS);
    // Skybox end ----------------

    // Transparent
    std::vector<RenderCommand::Ptr> transparentCommands = m_CommandBuffer->GetTransparentCommands();
    for (size_t i = 0; i < transparentCommands.size(); ++i)
    {
       RenderCommand::Ptr command = transparentCommands[i];
       RenderCommand(command);
    }

    // Debugging AABB
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    m_DebuggingAABBMat->Use();
    SetGLCull(!m_DebuggingAABBMat->GetDoubleSided());
    std::vector<RenderCommand::Ptr> commands = m_CommandBuffer->GetDebuggingCommands();
    for (size_t i = 0; i < commands.size(); ++i)
    {
        m_DebuggingAABBMat->SetMatrix("uModelMatrix", commands[i]->Transform);
        RenderMesh(commands[i]->Mesh);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    Blitter::BlitToCamera(m_IntermediateRT->GetColorTexture(0), m_RenderSize);
}

void SceneRenderGraph::RenderCommand(RenderCommand::Ptr command)
{
    Mesh::Ptr mesh = command->Mesh;
    Material::Ptr mat = command->Material;

    SetGLCull(!mat->GetDoubleSided());
    SetGLBlend(mat->GetAlphaMode() == Material::AlphaMode::BLEND);

    mat->AddOrSetTextureCube(m_EnvIBL->GetIrradiance());
    mat->AddOrSetTextureCube(m_EnvIBL->GetPrefiltered());
    mat->AddOrSetTexture(m_EnvIBL->GetBRDFLUTTexture());

    if (mat->GetMaterialCastShadows())
    {
        mat->AddOrSetTexture(m_ShadowmapRT->GetShadowmapTexture());
    }

    mat->Use();
    mat->SetMatrix("uModelMatrix", command->Transform);
    mat->SetMatrix("uModelMatrixInverse", glm::mat3x3(glm::inverse(command->Transform)));

    RenderMesh(mesh);
}

void SceneRenderGraph::RenderMesh(Mesh::Ptr mesh)
{
    glBindVertexArray(mesh->GetVertexArrayID());

    if (mesh->GetIndicesCount() > 0)
    {
        glDrawElements(GL_TRIANGLES, mesh->GetIndicesCount(), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, mesh->GetVerticesCount());
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

void SceneRenderGraph::SetGLCull(bool enable)
{
    if (m_CullFace != enable)
    {
        m_CullFace = enable;
        if (enable)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
    }
}

void SceneRenderGraph::SetGLBlend(bool enable)
{
    if (m_Blend != enable)
    {
        m_Blend = enable;
        if (enable)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    }
}
