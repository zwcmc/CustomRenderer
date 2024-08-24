#version 410 core
out vec4 OutColor;

void main()
{
    OutColor = vec4(vec3(gl_FragCoord.z), 1.0);
}