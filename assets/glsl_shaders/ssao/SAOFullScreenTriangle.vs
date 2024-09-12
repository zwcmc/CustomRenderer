#version 410 core

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
        1.0, // Set depth to a max of 1.0 for depth testing to skip pixels at max 1.0 (i.e. skybox)
        1.0
    );
}