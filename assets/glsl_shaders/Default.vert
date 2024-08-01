#version 410 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexcoord0;
layout (location = 2) in vec3 vNormal;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec2 UV0;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    UV0 = vTexcoord0;
    Normal = mat3(transpose(inverse(uModelMatrix))) * vNormal;

    FragPos = vec3(uModelMatrix * vec4(vPosition, 1.0));

    gl_Position = uProjectionMatrix * uViewMatrix * vec4(FragPos, 1.0);
}