#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in int selected;
layout(location = 3) in int visible;

out vec4 vColor;
out float vVisible;
out float vSelected;

uniform mat4 mvp_matrix;
uniform int size;
uniform float alpha;

void main()
{
    gl_Position = mvp_matrix * vec4(position, 1.0);
    gl_PointSize = selected == 1 ? max(1, int(size / 2)) : size;
    vColor = alpha != -1 ? vec4(color.rgb, alpha) : color;
    vVisible = float(visible);
    vSelected = float(selected);
}
