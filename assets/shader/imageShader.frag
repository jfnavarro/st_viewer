#version 330

uniform sampler2D tex;

in vec2 v_texcoord;

out vec4 fColor;

void main()
{
    // Set fragment color from texture
    fColor = texture(tex, v_texcoord);
}
