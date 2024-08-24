#include "utility/Collision.h"

#include <glm/gtc/type_ptr.hpp>

const glm::vec3 g_BoxCornerOffset[8] =
{
    glm::vec3(-1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f),

    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, 1.0f, -1.0f),
    glm::vec3(-1.0f, 1.0f, -1.0f)
};

namespace Collision
{
    //----------------------------------------------------------------
    // Axis-aligned bounding box
    //----------------------------------------------------------------
    std::vector<vec3> BoundingBox::GetCorners()
    {
        std::vector<vec3> corners;
        corners.resize(CORNER_COUNT);
        for (size_t i = 0; i < CORNER_COUNT; ++i)
        {
            glm::vec3 c = Center + g_BoxCornerOffset[i] * Extents;
            corners[i] = c;
        }
        return corners;
    }

    void BoundingBox::MergeBoundingBox(const BoundingBox &b, const glm::mat4 &transform)
    {
        vec3 bCenter = b.Center;
        vec3 bExtents = b.Extents;
        
        vec3 bMin = bCenter - bExtents;
        vec3 bMax = bCenter + bExtents;
        
        bMin = glm::make_vec3(transform * vec4(bMin, 1.0f));
        bMax = glm::make_vec3(transform * vec4(bMax, 1.0f));
        
        bMin = glm::min(Center - Extents, bMin);
        bMax = glm::max(Center + Extents, bMax);
        
        Center = (bMin + bMax) * 0.5f;
        Extents = (bMax - bMin) * 0.5f;
    }

    void BoundingBox::CreateMerged(BoundingBox &outBox, const BoundingBox &b1, const BoundingBox &b2)
    {
        vec3 b1Center = b1.Center;
        vec3 b1Extents = b1.Extents;
        
        vec3 b2Center = b2.Center;
        vec3 b2Extents = b2.Extents;
        
        vec3 mins = b1Center - b1Extents;
        mins = glm::min(mins, b2Center - b2Extents);
        
        vec3 maxs = b1Center + b1Extents;
        maxs = glm::max(maxs, b2Center + b2Extents);
        
        outBox.Center = (mins + maxs) * 0.5f;
        outBox.Extents = (maxs - mins) * 0.5f;
    }

    void BoundingBox::CreateFromPoints(BoundingBox &outBox, const vec3 &bbMin, const vec3 &bbMax)
    {
        vec3 min = glm::min(bbMin, bbMax);
        vec3 max = glm::max(bbMin, bbMax);
        
        outBox.Center = (min + max) * 0.5f;
        outBox.Extents = (max - min) * 0.5f;
    }

    //----------------------------------------------------------------
    // Bounding frustum
    //----------------------------------------------------------------
    std::vector<vec3> BoundingFrustum::GetCorners()
    {
        // Load origin
        vec3 vOrigin = Origin;
        
        // Build the corners of the frustum
        vec3 vRightTop = vec3(RightSlope, TopSlope, 1.0f);
        vec3 vRightBottom = vec3(RightSlope, BottomSlope, 1.0f);
        vec3 vLeftTop = vec3(LeftSlope, TopSlope, 1.0f);
        vec3 vLeftBottom = vec3(LeftSlope, BottomSlope, 1.0f);
        
        vec3 vNear = vec3(Near);
        vec3 vFar = vec3(Far);
        
        std::vector<vec3> vCorners;
        vCorners.resize(CORNER_COUNT);
        
        // Returns 8 corners position of bounding frustum.
        //     Near    Far
        //    0----1  4----5
        //    |    |  |    |
        //    |    |  |    |
        //    3----2  7----6
        
        vCorners[0] = vLeftTop * vNear;
        vCorners[1] = vRightTop * vNear;
        vCorners[2] = vRightBottom * vNear;
        vCorners[3] = vLeftBottom * vNear;
        vCorners[4] = vLeftTop * vFar;
        vCorners[5] = vRightTop * vFar;
        vCorners[6] = vRightBottom * vFar;
        vCorners[7] = vLeftBottom * vFar;
        
        for (size_t i = 0; i < CORNER_COUNT; ++i)
            vCorners[i] += Origin;
        
        return vCorners;
    }

    BoundingFrustum::BoundingFrustum(const glm::mat4 &projection)
    {
        CreateFromMatrix(*this, projection);
    }

    void BoundingFrustum::CreateFromMatrix(BoundingFrustum &outFrustum, const glm::mat4 &projection)
    {
        static vec4 NDCPoints[6] =
        {
                                          // Points at far plane
            vec4(1.0f, 0.0f, 1.0f, 1.0f),      // right
            vec4(-1.0f, 0.0f, 1.0f, 1.0f),     // left
            vec4(0.0f, 1.0f, 1.0f, 1.0f),      // top
            vec4(0.0f, -1.0f, 1.0f, 1.0f),     // bottom
            
            vec4(0.0f, 0.0f, -1.0f, 1.0f),      // near
            vec4(0.0f, 0.0f, 1.0f, 1.0f)       // far
        };
        
        glm::mat4 matInverse = glm::inverse(projection);
        
        // Compute the frustum corners in view space
        vec4 points[6];
        for (size_t i = 0; i < 6; ++i)
        {
            points[i] = matInverse * NDCPoints[i];
        }
        
        outFrustum.Origin = vec3(0.0f);
        outFrustum.Orientation = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        
        // Compute the slopes
        points[0] /= points[0].z;
        points[1] /= points[1].z;
        points[2] /= points[2].z;
        points[3] /= points[3].z;
        
        outFrustum.RightSlope = points[0].x;
        outFrustum.LeftSlope = points[1].x;
        outFrustum.TopSlope = points[2].y;
        outFrustum.BottomSlope = points[3].y;
        
        // Compute near and far
        points[4] /= points[4].w;
        points[5] /= points[5].w;
        
        outFrustum.Near = points[4].z;
        outFrustum.Far = points[5].z;
    }
}
