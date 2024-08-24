#pragma once

#include <vector>
#include <glm/glm.hpp>

using namespace glm;

namespace Collision
{
    // Axis-aligned bounding box
    struct BoundingBox
    {
        static constexpr size_t CORNER_COUNT = 8;

        vec3 Center;      // Center of the box.
        vec3 Extents;     // Distance from the center to each side.
        
        BoundingBox() noexcept : Center(0.0f, 0.0f, 0.0f), Extents(1.0f, 1.0f, 1.0f) { }

        // Get the 8 corners of the box
        std::vector<vec3> GetCorners();
        
        // Merge other bounding box
        void MergeBoundingBox(const BoundingBox &b, const glm::mat4 &transform = glm::mat4(1.0f));
        
        // Create axis-aligned box that contains two other bounding boxes
        static void CreateMerged(BoundingBox &outBox, const BoundingBox &b1, const BoundingBox &b2);
        
        // Create axis-aligned box from min/max points
        static void CreateFromPoints(BoundingBox &outBox, const vec3 &bbMin, const vec3 &bbMax);
    };
} // namespace Collision
