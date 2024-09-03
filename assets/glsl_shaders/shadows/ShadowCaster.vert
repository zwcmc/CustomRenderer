#version 410 core

layout (location = 0) in vec3 vPosition;

uniform mat4 uLightMVP;

void main()
{
    gl_Position = uLightMVP * vec4(vPosition, 1.0);

    // Shadow Pancaking
    gl_Position.z = max(gl_Position.z, -1.0);
}