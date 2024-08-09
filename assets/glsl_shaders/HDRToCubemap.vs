#version 410 core

layout (location = 0) in vec3 vPosition;

out vec3 WorldPos;

uniform mat4 vv;
uniform mat4 pp;

void main()
{
    WorldPos = vPosition;

    vec4 clipPos = pp * mat4(mat3(vv)) * vec4(vPosition, 1.0);

    gl_Position = vec4(clipPos.xy, 0.0, 1.0);
}