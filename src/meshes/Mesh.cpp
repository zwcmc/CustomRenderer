#include "meshes/Mesh.h"

Mesh::Mesh(const std::vector<vec3> &vertices, const std::vector<vec3> &normals, const std::vector<vec4> &tangents, const std::vector<vec2> &texcoords0, const std::vector<unsigned int> &indices)
    : m_VertexArrayID(0), m_VertexBufferID(0), m_ElementBufferID(0)
{
    m_Vertices = vertices;
    m_Normals = normals;
    m_Tangents = tangents;
    m_Texcoords = texcoords0;
    m_Indices = indices;
    InitBuffers();
}

Mesh::~Mesh()
{
    if (m_VertexArrayID)
    {
        glDeleteVertexArrays(1, &m_VertexArrayID);
        glDeleteBuffers(1, &m_VertexBufferID);
        glDeleteBuffers(1, &m_ElementBufferID);
    }
}

void Mesh::InitMesh(const std::vector<vec3> &vertices, const std::vector<unsigned int> &indices)
{
    m_VertexArrayID = 0;
    m_VertexBufferID = 0;
    m_ElementBufferID = 0;

    m_Vertices = vertices;
    m_Indices = indices;

    InitBuffers();
}

void Mesh::InitBuffers()
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

        if (m_Normals.size() > 0)
        {
            data.push_back(m_Normals[i].x);
            data.push_back(m_Normals[i].y);
            data.push_back(m_Normals[i].z);
        }
        
        if (m_Tangents.size() > 0)
        {
            data.push_back(m_Tangents[i].x);
            data.push_back(m_Tangents[i].y);
            data.push_back(m_Tangents[i].z);
            data.push_back(m_Tangents[i].w);
        }
        
        if (m_Texcoords.size() > 0)
        {
            data.push_back(m_Texcoords[i].x);
            data.push_back(m_Texcoords[i].y);
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
    if (m_Normals.size() > 0) stride += 3 * sizeof(float);
    if (m_Tangents.size() > 0) stride += 4 * sizeof(float);
    if (m_Texcoords.size() > 0) stride += 2 * sizeof(float);

    size_t offset = 0;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
    offset += 3 * sizeof(float);

    if (m_Normals.size() > 0)
    {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid *)offset);
         offset += 3 * sizeof(float);
    }
    
    if (m_Tangents.size() > 0)
    {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid *)offset);
         offset += 4 * sizeof(float);
    }
    
    if (m_Texcoords.size() > 0)
    {
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
//        offset += 2 * sizeof(float);
    }

    glBindVertexArray(0);
}
