#include "meshes/Sphere.h"

#include <glm/glm.hpp>
#include "defines.h"

Sphere::Sphere(const unsigned int &xSegments, const unsigned int &ySegments, const float &scale)
{
    std::vector<vec3> sphereVertices;
    for (unsigned int y = 0; y <= ySegments; ++y)
    {
        for (unsigned int x = 0; x <= xSegments; ++x)
        {
            float xSegment = (float)x / (float)xSegments;
            float ySegment = (float)y / (float)ySegments;
            float xPos = std::cos(xSegment * (float)M_TAU) * std::sin(ySegment * (float)M_PI) * scale; // TAU is 2PI
            float yPos = std::cos(ySegment * (float)M_PI) * scale;
            float zPos = std::sin(xSegment * (float)M_TAU) * std::sin(ySegment * (float)M_PI) * scale;

            sphereVertices.push_back(glm::vec3(xPos, yPos, zPos));
        }
    }

    std::vector<unsigned int> sphereIndices;
    for (unsigned int y = 0; y < ySegments; ++y)
    {
        for (unsigned int x = 0; x < xSegments; ++x)
        {
            sphereIndices.push_back((y + 1) * (xSegments + 1) + x);
            sphereIndices.push_back(y * (xSegments + 1) + x);
            sphereIndices.push_back(y * (xSegments + 1) + x + 1);

            sphereIndices.push_back((y + 1) * (xSegments + 1) + x);
            sphereIndices.push_back(y * (xSegments + 1) + x + 1);
            sphereIndices.push_back((y + 1) * (xSegments + 1) + x + 1);
        }
    }

    InitMesh(sphereVertices, sphereIndices);
}