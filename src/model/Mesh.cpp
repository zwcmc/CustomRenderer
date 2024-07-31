#include "model/Mesh.h"

Mesh::Mesh(const std::vector<vec3> &vertices)
    : m_VertexArrayID(0), m_VertexBufferID(0), m_ElementBufferID(0)
{
    m_Vertices = vertices;
    initBuffers();
}

Mesh::Mesh(const std::vector<vec3> &vertices, const std::vector<vec2> &texcoords)
    : m_VertexArrayID(0), m_VertexBufferID(0), m_ElementBufferID(0)
{
    m_Vertices = vertices;
    m_Texcoords = texcoords;
    initBuffers();
}

Mesh::Mesh(const std::vector<vec3> &vertices, const std::vector<unsigned int> &indices)
    : m_VertexArrayID(0), m_VertexBufferID(0), m_ElementBufferID(0)
{
    m_Vertices = vertices;
    m_Indices = indices;
    initBuffers();
}

Mesh::Mesh(const std::vector<vec3> &vertices, const std::vector<vec2> &texcoords, const std::vector<unsigned int> &indices)
    : m_VertexArrayID(0), m_VertexBufferID(0), m_ElementBufferID(0)
{
    m_Vertices = vertices;
    m_Texcoords = texcoords;
    m_Indices = indices;
    initBuffers();
}

Mesh::Mesh(const std::vector<vec3> &vertices, const std::vector<vec2> &texcoords, const std::vector<vec3> &normals, const std::vector<unsigned int> &indices)
    : m_VertexArrayID(0), m_VertexBufferID(0), m_ElementBufferID(0)
{
    m_Vertices = vertices;
    m_Texcoords = texcoords;
    m_Normals = normals;
    m_Indices = indices;
    initBuffers();
}

void Mesh::initBuffers()
{
    if (!m_VertexArrayID)
    {
        glGenVertexArrays(1, &m_VertexArrayID);
        glGenBuffers(1, &m_VertexBufferID);
        glGenBuffers(1, &m_ElementBufferID);
    }

    std::vector<float> data;
    for (int i = 0; i < m_Vertices.size(); ++i)
    {
        data.push_back(m_Vertices[i].x);
        data.push_back(m_Vertices[i].y);
        data.push_back(m_Vertices[i].z);

        if (m_Texcoords.size() > 0)
        {
            data.push_back(m_Texcoords[i].x);
            data.push_back(m_Texcoords[i].y);
        }

        if (m_Normals.size() > 0)
        {
            data.push_back(m_Normals[i].x);
            data.push_back(m_Normals[i].y);
            data.push_back(m_Normals[i].z);
        }
    }

    glBindVertexArray(m_VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

    if (m_Indices.size() > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementBufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);
    }

    GLsizei stride = 3 * sizeof(float);
    if (m_Texcoords.size() > 0) stride += 2 * sizeof(float);
    if (m_Normals.size() > 0) stride += 3 * sizeof(float);

    size_t offset = 0;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
    offset += 3 * sizeof(float);

    if (m_Texcoords.size() > 0)
    {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
        offset += 2 * sizeof(float);
    }

    if (m_Normals.size() > 0)
    {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid *)offset);
        offset += 3 * sizeof(float);
    }

    glBindVertexArray(0);
}