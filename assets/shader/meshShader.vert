#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 mvp_matrix;

void main()
{
    gl_Position = mvp_matrix * vec4(position, 1.0);
    //vNormal = normal;
}
