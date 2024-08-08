#include "meshes/TriangleCS.h"

TriangleCS::TriangleCS()
{
    std::vector<vec3> quadVertices =
    {
        vec3(-1.0f, -1.0f, 0.0f),
        vec3(3.0f, -1.0f, 0.0f),
        vec3(-1.0f, 3.0f, 0.0f)
    };

    initMesh(quadVertices);
}