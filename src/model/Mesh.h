#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

#include "ptr.h"

using namespace glm;

class Mesh
{
SHARED_PTR(Mesh)
public:
    Mesh() = default;
    Mesh(const std::vector<vec3> &vertices);
    Mesh(const std::vector<vec3> &vertices, const std::vector<vec2> &texcoords);
    Mesh(const std::vector<vec3> &vertices, const std::vector<unsigned int> &indices);
    Mesh(const std::vector<vec3> &vertices, const std::vector<vec2> &texcoords, const std::vector<unsigned int> &indices);
    Mesh(const std::vector<vec3> &vertices, const std::vector<vec2> &texcoords, const std::vector<vec3> &normals, const std::vector<unsigned int> &indices);

    inline GLuint getVertexArrayID() { return m_VertexArrayID; }
    inline GLsizei getIndicesCount() { return static_cast<GLsizei>(m_Indices.size()); }
    inline GLsizei getVerticesCount() { return static_cast<GLsizei>(m_Vertices.size()); }

private:
    void initBuffers();

    GLuint m_VertexArrayID, m_VertexBufferID, m_ElementBufferID;

    std::vector<vec3> m_Vertices;
    std::vector<vec2> m_Texcoords;
    std::vector<vec3> m_Normals;
    std::vector<unsigned int> m_Indices;
};