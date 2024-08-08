#version 410 core

layout (location = 0) in vec3 vPosition;

out vec2 UV0;

void main()
{
    UV0 = vec2(
        gl_VertexID == 1 ? 2.0 : 0.0,
        gl_VertexID == 2 ? 2.0 : 0.0
    );
    gl_Position = vec4(
        gl_VertexID == 1 ? 3.0 : -1.0,
        gl_VertexID == 2 ? 3.0 : -1.0,
        0.0,
        1.0
    );
}