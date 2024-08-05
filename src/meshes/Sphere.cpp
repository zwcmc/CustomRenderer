#include "meshes/Sphere.h"

#include <glm/glm.hpp>
#include "globals.h"

Sphere::Sphere(unsigned int xSegments, unsigned int ySegments)
{
    std::vector<vec3> sphereVertices;
    for (unsigned int y = 0; y <= ySegments; ++y)
    {
        for (unsigned int x = 0; x <= xSegments; ++x)
        {
            float xSegment = (float)x / (float)xSegments;
            float ySegment = (float)y / (float)ySegments;
            float xPos = std::cos(xSegment * M_TAU) * std::sin(ySegment * M_PI); // TAU is 2PI
            float yPos = std::cos(ySegment * M_PI);
            float zPos = std::sin(xSegment * M_TAU) * std::sin(ySegment * M_PI);

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

    initMesh(sphereVertices, sphereIndices);
}