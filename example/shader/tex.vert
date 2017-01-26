#version 150

in vec3 position;
in vec2 uv;
uniform mat4 projection;

out vec2 vUV;

void main()
{
    gl_Position = projection * vec4(position, 1.0f);
    vUV = uv;
}