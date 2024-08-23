#include "AABBCube.h"

AABBCube::AABBCube(const glm::vec3 &aabbMin, const glm::vec3 &aabbMax)
{
    std::vector<glm::vec3> vertices =
    {
        glm::vec3(aabbMin.x, aabbMin.y, aabbMin.z),
        glm::vec3(aabbMax.x, aabbMin.y, aabbMin.z),
        glm::vec3(aabbMax.x, aabbMax.y, aabbMin.z),
        glm::vec3(aabbMin.x, aabbMax.y, aabbMin.z),
        glm::vec3(aabbMin.x, aabbMin.y, aabbMax.z),
        glm::vec3(aabbMax.x, aabbMin.y, aabbMax.z),
        glm::vec3(aabbMax.x, aabbMax.y, aabbMax.z),
        glm::vec3(aabbMin.x, aabbMax.y, aabbMax.z),
    };

    std::vector<unsigned int> indices =
    {
        0,1,2,  2,3,0, // Face 1
        4,5,6,  6,7,4, // Face 2
        0,1,5,  5,4,0, // Face 3
        2,3,7,  7,6,2, // Face 4
        0,3,7,  7,4,0, // Face 5
        1,2,6,  6,5,1  // Face 6
    };

    initMesh(vertices, indices);
}