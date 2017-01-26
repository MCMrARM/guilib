#version 150

uniform sampler2D textureSampler;

in vec2 vUV;
out vec4 oColor;

void main()
{
    oColor = texture(textureSampler, vUV);
}