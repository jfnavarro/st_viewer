#version 330

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord;

uniform mat4 mvp_matrix;

out vec2 v_texcoord;

void main()
{
    gl_Position = mvp_matrix * vec4(a_position, 1.0);
    v_texcoord = a_texcoord;
}
