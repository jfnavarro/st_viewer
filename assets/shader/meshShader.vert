#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec4 vColor;

uniform mat4 mvp_matrix;
uniform vec4 color;

void main()
{
    gl_Position = mvp_matrix * vec4(position, 1.0);
    vColor = color;
}
