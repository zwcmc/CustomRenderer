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
}
