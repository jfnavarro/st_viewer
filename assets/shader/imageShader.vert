#version 330

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texcoord;

uniform mat4 mvp_matrix;

out vec2 v_texcoord;

void main()
{
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * a_position;

    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    v_texcoord = a_texcoord;
}
