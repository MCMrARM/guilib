#version 150

in vec3 position;
in vec4 color;
uniform mat4 projection;

out vec4 vColor;

void main()
{
    gl_Position = projection * vec4(position, 1.0f);
    vColor = color;
}