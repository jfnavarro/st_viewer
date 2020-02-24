#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 vNormal;
out vec3 vFragPos;

//uniform mat4 mvp_matrix;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    //gl_Position = mvp_matrix * vec4(position, 1.0);
    gl_Position = projection * view * model * vec4(position, 1.0);
    vFragPos = vec3(model * vec4(position, 1.0));
    vNormal = normal;
}
