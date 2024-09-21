#version 410 core

in vec2 UV0;

uniform sampler2D uDepthAttachment;

void main()
{
    gl_FragDepth = texture(uDepthAttachment, UV0).r;
}